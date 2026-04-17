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
	bool isSameAs(const Type_Level_2FECE &t, const type_entity_0x30311 *e) const {
		return (memcmp(&terrain, &t, sizeof(Type_Level_2FECE)) == 0) &&
				(memcmp(entities, e, sizeof(type_entity_0x30311) * 1200) == 0);
	}
};

class UndoRedoManager {
public:
	explicit UndoRedoManager(Type_Level_2FECE &terrain, type_entity_0x30311 *entities, size_t maxSteps = 100) :
			m_currentTerrain(terrain), m_currentEntities(entities), m_maxSteps(maxSteps) {
		// Uložíme počáteční stav obou objektů
		saveToStack(undoStack);
	}

	void saveState() {
		// Kontrola shody s posledním stavem (porovnáváme obojí)
		if (!undoStack.empty() && undoStack.back().isSameAs(m_currentTerrain, m_currentEntities)) {
			return;
		}

		redoStack.clear();
		saveToStack(undoStack);

		if (undoStack.size() > m_maxSteps + 1) {
			undoStack.pop_front();
		}
	}

	bool undo() {
		if (undoStack.size() <= 1)
			return false;

		redoStack.push_back(undoStack.back());
		undoStack.pop_back();

		restoreFromSnapshot(undoStack.back());
		return true;
	}

	bool redo() {
		if (redoStack.empty())
			return false;

		undoStack.push_back(redoStack.back());
		restoreFromSnapshot(undoStack.back());
		redoStack.pop_back();

		return true;
	}

	// Pomocné metody pro přístup k informacím
	bool canUndo() const { return undoStack.size() > 1; }
	bool canRedo() const { return !redoStack.empty(); }

private:
	// Reference na originální data v editoru
	Type_Level_2FECE &m_currentTerrain;
	type_entity_0x30311 *m_currentEntities;

	std::deque<EditorSnapshot> undoStack;
	std::deque<EditorSnapshot> redoStack;
	size_t m_maxSteps;

	// Vytvoří kopii aktuálních dat do zásobníku
	void saveToStack(std::deque<EditorSnapshot> &stack) {
		EditorSnapshot snapshot;
		snapshot.terrain = m_currentTerrain;
		memcpy(snapshot.entities, m_currentEntities, sizeof(type_entity_0x30311) * 1200);
		stack.push_back(snapshot);
	}

	// Přepíše data v editoru daty ze snapshotu
	void restoreFromSnapshot(const EditorSnapshot &snapshot) {
		m_currentTerrain = snapshot.terrain;
		memcpy(m_currentEntities, snapshot.entities, sizeof(type_entity_0x30311) * 1200);
	}
};


//extern Type_Level_2FECE tempTerrain;
extern type_entity_0x30311 temparray_0x30311[];

extern std::unique_ptr<UndoRedoManager> urManager;

void editor_run();
void main_x();

#endif
