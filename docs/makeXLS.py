import os
from openpyxl import Workbook
from openpyxl.drawing.image import Image

folder = "c:\\Users\\vesely\\AppData\\Roaming\\Godot\\app_userdata\\MagicBalls\\convertdata\\TMAPS\\TMAPS-compare\\"
files = sorted(os.listdir(folder))

wb = Workbook()
ws = wb.active

row = 1

for file in files:
    if file.lower().endswith((".png", ".jpg", ".jpeg", ".bmp")):
        img_path = os.path.join(folder, file)
        img = Image(img_path)

        img.anchor = f"A{row}"
        ws.add_image(img)

        ws.row_dimensions[row].height = 100
        row += 1

wb.save("obrazky.xlsx")
