#pragma once
#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "../remc2/engine/EventsFunctions.h"
#include "../remc2/sub_main.h"

#include <memory>
#include <deque>
#include <vector>

struct EditorSnapshot {
	Type_Level_2FECE terrain;
	type_entity_0x30311 entities[1200];

	// Operátor pro snadné porovnání stavů (náhrada memcmp)
	bool isSameAs(const Type_Level_2FECE &t) const {
		return (memcmp(&terrain, &t, sizeof(Type_Level_2FECE)) == 0);
	}
};

class UndoRedoManager {
public:
	explicit UndoRedoManager(size_t maxSteps = 100) :
			m_maxSteps(maxSteps) {
	}

	void init(Type_Level_2FECE &terrain) {
		undoStack.clear();
		redoStack.clear();
		saveToStack(undoStack, terrain);
	}

    void saveState(Type_Level_2FECE &terrain) {
		if (!undoStack.empty() && undoStack.back().isSameAs(terrain)) {
			return;
		}
		redoStack.clear();
		saveToStack(undoStack, terrain);
		if (undoStack.size() > m_maxSteps + 1) {
			undoStack.pop_front();
		}
	}

    bool undo(Type_Level_2FECE &terrain) {
		if (undoStack.size() <= 1)
			return false;
		redoStack.push_back(undoStack.back());
		undoStack.pop_back();
		restoreFromSnapshot(undoStack.back(), terrain);
		return true;
	}

	bool redo(Type_Level_2FECE &terrain) {
		if (redoStack.empty())
			return false;
		undoStack.push_back(redoStack.back());
		restoreFromSnapshot(undoStack.back(), terrain);
		redoStack.pop_back();
		return true;
	}

	bool canUndo() const { return undoStack.size() > 1; }
	bool canRedo() const { return !redoStack.empty(); }

private:
	std::deque<EditorSnapshot> undoStack;
	std::deque<EditorSnapshot> redoStack;
	size_t m_maxSteps;

    void saveToStack(std::deque<EditorSnapshot> &stack, const Type_Level_2FECE &terrain) {
		EditorSnapshot snapshot;
		snapshot.terrain = terrain;
		stack.push_back(snapshot);
	}

    void restoreFromSnapshot(const EditorSnapshot &snapshot, Type_Level_2FECE &terrain) {
		terrain = snapshot.terrain;
	}
};


extern Type_Level_2FECE tempTerrain;
//extern type_entity_0x30311 temparray_0x30311[];

extern std::unique_ptr<UndoRedoManager> urManager;

void editor_run();
void main_x();
void loadlevel(int levelnumber);

#endif
