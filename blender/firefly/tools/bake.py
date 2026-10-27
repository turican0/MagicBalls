# Bakes the reference photos straight into the firefly's UV maps: every texel gets its 3D point
# (rest pose, mesh.npz from bl_dump.py), which is projected into the photo from above (top view)
# or below (ventral view). Each part is morphed into its own region of the photo row by row:
# the body axis is mapped piecewise linearly through landmarks and across it the width of the
# part on the model is stretched to the width of the same part on the photo (masks of the photo).
import sys, os
import numpy as np
from PIL import Image
from scipy.ndimage import gaussian_filter, binary_opening, median_filter, distance_transform_edt, rotate

S = sys.path[0]
OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)
D = 'C:/Users/vesely/Downloads/'
mesh = np.load(f'{S}/mesh.npz')

def load(p):
    return np.asarray(Image.open(p).convert('RGB')).astype(np.float32) / 255.0

TOP = load(D + '28c31d0c-eb7a-420d-bd80-d1b0eb7c2356.png')   # dorsal view, head up
VEN = load(D + '1c12e346-22cc-436c-8342-bde02f632529.png')   # "Back" = ventral view, head up
OLD = f'{S}/tex'

def lum(a):
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114

def sample(img, x, y):
    # bilinear
    H, W = img.shape[:2]
    x = np.clip(x, 0, W - 1.001); y = np.clip(y, 0, H - 1.001)
    x0 = np.floor(x).astype(int); y0 = np.floor(y).astype(int)
    fx = (x - x0)[..., None]; fy = (y - y0)[..., None]
    a = img[y0, x0] * (1 - fx) + img[y0, x0 + 1] * fx
    b = img[y0 + 1, x0] * (1 - fx) + img[y0 + 1, x0 + 1] * fx
    return a * (1 - fy) + b * fy

# ---------------------------------------------------------------- texel -> 3D point
def raster(keys, W, H):
    pos = np.zeros((H, W, 3), np.float32); nrm = np.zeros((H, W, 3), np.float32)
    obj = np.full((H, W), -1, np.int16)
    for oi, k in enumerate(keys):
        for t in mesh[k]:
            u = t[:, 0] * W - 0.5; v = (1 - t[:, 1]) * H - 0.5
            x0, x1 = int(max(np.floor(u.min()), 0)), int(min(np.ceil(u.max()), W - 1))
            y0, y1 = int(max(np.floor(v.min()), 0)), int(min(np.ceil(v.max()), H - 1))
            if x1 < x0 or y1 < y0:
                continue
            den = (v[1] - v[2]) * (u[0] - u[2]) + (u[2] - u[1]) * (v[0] - v[2])
            if abs(den) < 1e-12:
                continue
            ys, xs = np.mgrid[y0:y1 + 1, x0:x1 + 1]
            w0 = ((v[1] - v[2]) * (xs - u[2]) + (u[2] - u[1]) * (ys - v[2])) / den
            w1 = ((v[2] - v[0]) * (xs - u[2]) + (u[0] - u[2]) * (ys - v[2])) / den
            w2 = 1 - w0 - w1
            m = (w0 >= -1e-4) & (w1 >= -1e-4) & (w2 >= -1e-4)
            if not m.any():
                continue
            w = np.stack([w0[m], w1[m], w2[m]], -1)
            pos[ys[m], xs[m]] = w @ t[:, 2:5]
            nn = w @ t[:, 5:8]
            nrm[ys[m], xs[m]] = nn / (np.linalg.norm(nn, axis=-1, keepdims=True) + 1e-9)
            obj[ys[m], xs[m]] = oi
    return pos, nrm, obj

def bleed(img, mask):
    # fills the texels outside the UV islands with the nearest island colour (no seams in mips)
    idx = distance_transform_edt(~mask, return_distances=False, return_indices=True)
    return img[idx[0], idx[1]]

# ---------------------------------------------------------------- outlines
def model_extent(keys, ymin, ymax, n=400, side=1):
    # left/right x of the parts seen from above (side 1) or below (side -1), per body row (Y)
    xs_l = np.full(n, np.nan); xs_r = np.full(n, np.nan)
    pts = np.concatenate([mesh[k][:, :, 2:5].reshape(-1, 3) for k in keys])
    # dense sampling of the triangles for the extents
    tr = np.concatenate([mesh[k][:, :, 2:5] for k in keys])
    r = np.random.RandomState(1).dirichlet((1, 1, 1), size=24)
    samp = np.einsum('sk,tkd->tsd', r, tr).reshape(-1, 3)
    samp = np.concatenate([samp, pts])
    bi = ((samp[:, 1] - ymin) / (ymax - ymin) * (n - 1)).round().astype(int)
    ok = (bi >= 0) & (bi < n)
    for b in range(n):
        s = samp[ok & (bi == b), 0]
        if s.size:
            xs_l[b], xs_r[b] = s.min(), s.max()
    for a in (xs_l, xs_r):
        good = ~np.isnan(a)
        a[~good] = np.interp(np.flatnonzero(~good), np.flatnonzero(good), a[good])
    return median_filter(xs_l, 9), median_filter(xs_r, 9)

def photo_extent(img, box, thr=0.075, open_r=4):
    # left/right x of the photo region (x0, y0, x1, y1) per photo row, thin legs removed
    x0, y0, x1, y1 = box
    sub = img[y0:y1, x0:x1]
    m = gaussian_filter(lum(sub), 1.0) > thr
    yy, xx = np.mgrid[-open_r:open_r + 1, -open_r:open_r + 1]
    m = binary_opening(m, structure=(xx * xx + yy * yy) <= open_r * open_r)
    L = np.full(y1 - y0, np.nan); R = np.full(y1 - y0, np.nan)
    for i in range(y1 - y0):
        c = np.flatnonzero(m[i])
        if c.size:
            L[i], R[i] = c.min() + x0, c.max() + x0
    for a in (L, R):
        good = ~np.isnan(a)
        a[~good] = np.interp(np.flatnonzero(~good), np.flatnonzero(good), a[good])
    return median_filter(L, 15), median_filter(R, 15)

class Morph:
    # model (X, Y) of a part -> photo (x, y): Y through landmarks [(Y, photo_y)], X by the widths
    def __init__(self, img, keys, marks, box, x_fixed=None, thr=0.075, open_r=4, inset=0.03):
        self.img = img
        self.marks = sorted(marks)
        self.ymin, self.ymax = self.marks[0][0], self.marks[-1][0]
        self.n = 400
        self.ml, self.mr = model_extent(keys, self.ymin, self.ymax, self.n)
        self.box = box
        self.pl, self.pr = photo_extent(img, box, thr, open_r)
        if x_fixed is not None: # one side of the part is a straight line on the photo (seam)
            side, x = x_fixed
            (self.pr if side == 'r' else self.pl)[:] = x
        self.inset = inset

    def map(self, X, Y):
        my = np.array([m[0] for m in self.marks]); py = np.array([m[1] for m in self.marks])
        y = np.interp(Y, my, py)
        b = np.clip(((Y - self.ymin) / (self.ymax - self.ymin) * (self.n - 1)).round().astype(int), 0, self.n - 1)
        u = (X - self.ml[b]) / np.maximum(self.mr[b] - self.ml[b], 1e-6)
        u = self.inset + np.clip(u, 0, 1) * (1 - 2 * self.inset)
        r = np.clip((y - self.box[1]).round().astype(int), 0, len(self.pl) - 1)
        x = self.pl[r] + u * (self.pr[r] - self.pl[r])
        return sample(self.img, x, y)

def old_img(i):
    for e in ('jpg', 'png'):
        p = f'{OLD}/img{i:02d}.{e}'
        if os.path.exists(p):
            return load(p)

def save(a, name, jpg=True):
    im = Image.fromarray(np.clip(a * 255 + 0.5, 0, 255).astype(np.uint8))
    p = f'{OUT}/{name}.' + ('jpg' if jpg else 'png')
    im.save(p, quality=93) if jpg else im.save(p)
    print('saved', p, im.size)

def normal_with_detail(old_n, col, strength, sigma=2.5):
    # the photo's own fine relief (high-passed brightness) on top of the old normal map
    g = lum(col)
    h = g - gaussian_filter(g, sigma * 4)
    h = gaussian_filter(h, 0.7)
    gy, gx = np.gradient(h)
    a = old_n * 2 - 1
    r = np.stack([a[..., 0] - gx * strength, a[..., 1] + gy * strength, a[..., 2]], -1)
    r /= np.linalg.norm(r, axis=-1, keepdims=True)
    return r * 0.5 + 0.5

def orm_from(col, mask, rough, rough_var, occl_old):
    g = lum(col)
    hp = g - gaussian_filter(g, 6)
    o = np.zeros(col.shape, np.float32)
    o[..., 0] = occl_old
    o[..., 1] = np.clip(rough - hp * rough_var, 0.1, 1.0)
    o[..., 2] = 0.0 # not metallic
    return o

def weights(nrm):
    # top / bottom blend by the normal (Z up)
    wt = np.clip(nrm[..., 2] * 2.0 + 0.5, 0, 1)
    return wt, 1 - wt

# landmarks: model Y -> photo row
# dorsal photo (TOP, 1536x1024 image, beetle at x 120-265)
TOP_PRON = [(0.308, 181.0), (0.106, 246.0)]
TOP_ELY = [(0.114, 246.0), (-0.507, 556.0)]
TOP_HEAD = [(0.335, 145.0), (0.20, 190.0)]
# ventral photo (VEN): head 145, front coxae 207, hind coxae 255, lantern 377-458
VEN_BODY = [(0.335, 140.0), (0.213, 207.0), (-0.03, 262.0), (-0.177, 377.0)]
VEN_LANT = [(-0.159, 377.0), (-0.5, 458.0)]
VEN_PRON = [(0.308, 150.0), (0.106, 215.0)]

K = {k.split('|')[0]: k for k in mesh.files}

# ---- elytra (both share M_Elytra; each elytron morphed into its own half of the photo)
W, H = 4096, 1024
pos, nrm, obj = raster([K['Elytra_L'], K['Elytra_R']], W, H)
mask = obj >= 0
mL = Morph(TOP, [K['Elytra_L']], TOP_ELY, (110, 240, 193, 560), x_fixed=('r', 191.0), inset=0.0)
mR = Morph(TOP, [K['Elytra_R']], TOP_ELY, (191, 240, 275, 560), x_fixed=('l', 192.0), inset=0.0)
# the UV islands are shared by both elytra (mirrored) - take the left one where both land
col = np.zeros((H, W, 3), np.float32)
sel = obj == 0
col[sel] = mL.map(pos[sel][:, 0], pos[sel][:, 1])
sel = obj == 1
col[sel] = mR.map(pos[sel][:, 0], pos[sel][:, 1])
# the underside of the elytra (faces down): dark brown with the gold rim of the photo dimmed
wt, wb = weights(nrm)
under = np.array([0.10, 0.075, 0.045], np.float32)
col = col * wt[..., None] + (col * 0.35 + under * 0.65) * wb[..., None]
col = bleed(col, mask)
save(col, 'elytra_color')
save(orm_from(col, mask, 0.42, 1.2, old_img(1)[..., 0]), 'elytra_orm')
save(normal_with_detail(old_img(2), col, 6.0), 'elytra_normal', jpg=False)

# ---- pronotum + head capsule (Head mesh, M_Pronotum)
W, H = 2048, 2048
pos, nrm, obj = raster([K['Head']], W, H)
mask = obj >= 0
mt = Morph(TOP, [K['Head']], TOP_PRON, (125, 176, 262, 252), inset=0.01)
mb = Morph(VEN, [K['Head']], VEN_PRON, (918, 145, 1022, 215), inset=0.02)
wt, wb = weights(nrm)
col = np.zeros((H, W, 3), np.float32)
X, Y = pos[mask][:, 0], pos[mask][:, 1]
col[mask] = mt.map(X, Y) * wt[mask][:, None] + mb.map(X, Y) * wb[mask][:, None]
col = bleed(col, mask)
save(col, 'pronotum_color')
save(orm_from(col, mask, 0.32, 1.0, old_img(4)[..., 0]), 'pronotum_orm')
save(normal_with_detail(old_img(5), col, 5.0), 'pronotum_normal', jpg=False)

# ---- body: head, thorax, abdomen
W, H = 1536, 1536
pos, nrm, obj = raster([K['Body']], W, H)
mask = obj >= 0
mv = Morph(VEN, [K['Body']], VEN_BODY, (895, 135, 1050, 380), open_r=6, inset=0.3)
mh = Morph(TOP, [K['Body']], TOP_HEAD, (150, 140, 240, 195), inset=0.02)
wt, wb = weights(nrm)
col = np.zeros((H, W, 3), np.float32)
X, Y = pos[mask][:, 0], pos[mask][:, 1]
ven = mv.map(X, Y)
# the back of the abdomen under the elytra is not on any photo: the ventral plates, darker
dors = ven * 0.6
head = mh.map(X, Y)
hw = np.clip((Y - 0.19) / 0.04, 0, 1) # the head from the dorsal photo
top = dors * (1 - hw[:, None]) + head * hw[:, None]
col[mask] = top * wt[mask][:, None] + ven * wb[mask][:, None]
col = bleed(col, mask)
save(col, 'body_color')
save(orm_from(col, mask, 0.45, 1.0, old_img(7)[..., 0]), 'body_orm')
save(normal_with_detail(old_img(8), col, 5.0), 'body_normal', jpg=False)

# ---- lantern: the light organ of the ventral photo, all round
W, H = 2048, 1024
pos, nrm, obj = raster([K['Lantern']], W, H)
mask = obj >= 0
ml = Morph(VEN, [K['Lantern']], VEN_LANT, (925, 372, 1020, 462), thr=0.4, open_r=3, inset=0.06)
col = np.zeros((H, W, 3), np.float32)
X, Y = pos[mask][:, 0], pos[mask][:, 1]
ph = ml.map(X, Y)
col[mask] = ph
col = bleed(col, mask)
# where it glows is taken from the original model (its emission mask, cells closed into one
# compact area); the photo only adds its fine detail to the glow and the albedo
from scipy.ndimage import binary_closing, binary_fill_holes
em_old = lum(old_img(12))
glow = gaussian_filter(em_old, 6) > 0.12
glow = binary_closing(glow, structure=np.ones((25, 25)))
glow = binary_fill_holes(glow)
# the light organ wraps around the sides of the same segments (seen glowing from the side too):
# along the body as far as the original mask goes, up to the upper edge of the flanks
y_front = np.percentile(pos[glow & mask][:, 1], 99)
# (the normal smoothed first, so the folds between the plates do not leave dark gaps)
nz = gaussian_filter(np.where(mask, nrm[..., 2], 0.0), 14) / np.maximum(gaussian_filter(mask.astype(np.float32), 14), 1e-3)
along = np.clip((y_front - pos[..., 1]) / 0.045, 0, 1) ** 1.5
side = along * mask # all round the last segments, the same above and below
glow = np.maximum(gaussian_filter(glow.astype(np.float32), 16), gaussian_filter(side.astype(np.float32), 10))
glow = np.clip(glow * 1.15, 0, 1)
g = lum(col)
det = np.clip((g + 0.05) / (gaussian_filter(g, 12) + 0.05), 0.75, 1.25)
emis = (glow ** 2.2)[..., None] * np.array([0.93, 1.0, 0.32], np.float32) * (0.8 + 0.2 * det)[..., None]
cream = np.array([0.80, 0.80, 0.52], np.float32) * det[..., None]
# outside the light organ: the translucent brownish plates of the photo
plates = col * np.array([0.55, 0.45, 0.3], np.float32)
alb = cream * glow[..., None] + plates * (1 - glow[..., None])
save(alb, 'lantern_color')
save(np.clip(emis, 0, 1), 'lantern_emission', jpg=False)
save(orm_from(alb, mask, 0.38, 1.0, old_img(10)[..., 0]), 'lantern_orm')
save(normal_with_detail(old_img(11), alb, 3.0), 'lantern_normal', jpg=False)

# ---- legs and antennae: straightened strips of the leg / antenna photos along the UV length
def strip(img, p0, p1, half_w, length_px):
    # samples the photo along the segment p0 -> p1, half_w pixels to both sides
    p0 = np.array(p0, np.float32); p1 = np.array(p1, np.float32)
    d = (p1 - p0) / np.linalg.norm(p1 - p0); n = np.array([-d[1], d[0]])
    t = np.linspace(0, 1, length_px)[:, None, None]
    s = np.linspace(-half_w, half_w, int(half_w * 2) + 1)[None, :, None]
    pts = p0 + (p1 - p0) * t + n * s
    return sample(img, pts[..., 0], pts[..., 1])

def wrap(stripimg, W, H):
    # strip (length x width) -> texture: v along the length, u around (mirrored, no seam)
    s = np.asarray(Image.fromarray(np.clip(stripimg * 255, 0, 255).astype(np.uint8)).resize((W // 2, H), Image.BICUBIC)).astype(np.float32) / 255
    return np.concatenate([s, s[:, ::-1]], 1)

A = load(D + '1f0e15b3-9df0-4556-a564-cf70b186c975.png')
# "Detail nohy": the tibia from the knee down to the tarsus
legs = strip(A, (1030, 440), (1095, 610), 9, 1024)
legs = wrap(legs, 256, 1024)
save(legs, 'legs_color')
save(orm_from(legs, None, 0.5, 1.0, old_img(17)[..., 0]), 'legs_orm')
save(normal_with_detail(old_img(18), legs, 4.0), 'legs_normal', jpg=False)
# "Detail tykadla": the segmented antenna
ant = strip(A, (1290, 550), (1500, 395), 7, 1024)
ant = wrap(ant, 128, 1024)
save(ant, 'antenna_color')
save(orm_from(ant, None, 0.45, 1.0, old_img(20)[..., 0]), 'antenna_orm')
save(normal_with_detail(old_img(21), ant, 4.0), 'antenna_normal', jpg=False)
