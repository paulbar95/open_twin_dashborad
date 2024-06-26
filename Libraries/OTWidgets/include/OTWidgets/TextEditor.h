//! @file TextEditor.h
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/PlainTextEdit.h"

// Qt header
#include <qstring.h>
#include <qstringlist.h>
#include <qsyntaxhighlighter.h>
#include <qregularexpression.h>

class QPaintEvent;
class QResizeEvent;
class QSyntaxHighlighter;

namespace ot {
	class TextEditor;
	class TextEditorSearchPopup;
	
	class OT_WIDGETS_API_EXPORT TextEditorLineNumberArea : public QWidget {
	public:
		TextEditorLineNumberArea(TextEditor* _editor);
		virtual ~TextEditorLineNumberArea();

		QSize sizeHint() const override;

	protected:
		void paintEvent(QPaintEvent * _event) override;

	private:
		TextEditor*	m_editor;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class OT_WIDGETS_API_EXPORT TextEditor : public PlainTextEdit {
		Q_OBJECT
		OT_DECL_NOCOPY(TextEditor)
	public:
		TextEditor(QWidget* _parent = (QWidget*)nullptr);
		virtual ~TextEditor();

		int lineNumberAreaWidth(void) const;
		void lineNumberAreaPaintEvent(QPaintEvent * _event);

		void setContentChanged(bool _changed = true) { m_contentChanged = _changed; };
		bool contentChanged(void) const { return m_contentChanged; };

		void setTextEditorName(const std::string& _name) { m_textEditorName = _name; };
		const std::string& textEditorName(void) const { return m_textEditorName; };

		void setCode(const QString& _text);
		void setCode(const QStringList& _lines);

		QStringList code(void) const;

		void setSyntaxHighlighter(QSyntaxHighlighter * _highlighter);
		QSyntaxHighlighter * syntaxHighlighter(void) { return m_syntaxHighlighter; }

		bool requiresRefreshing(ot::UID displayedTextEntityID, ot::UID displayedTextEntityVersion);

		void setTabSpaces(int _spaces) { m_tabSpaces = _spaces; };
		int tabSpaces(void) const { return m_tabSpaces; };

		void setNewLineWithSamePrefix(bool _enabled) { m_newLineSamePrefix = _enabled; };
		bool newLineWithSamePrefix(void) const { return m_newLineSamePrefix; };

		void setDuplicateLineShortcutEnabled(bool _enabled) { m_enableDuplicateLineShortcut = _enabled; };
		bool isDuplicateLineShortcutEnabled(void) const { return m_enableDuplicateLineShortcut; };

		void setEnableSameTextHighlighting(bool _enabled) { m_enableSameTextHighlighting = _enabled; };
		bool isSameTextHighlightingEnabled(void) const { return m_enableSameTextHighlighting; };

	Q_SIGNALS:							
		void saveRequested(void);

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void resizeEvent(QResizeEvent * _event) override;
		virtual void wheelEvent(QWheelEvent* _event) override;
		
	private Q_SLOTS:
		void slotUpdateLineNumberAreaWidth(int _newBlockCount);
		void slotHighlightCurrentLine();
		void slotUpdateLineNumberArea(const QRect & _rect, int _dy);
		void slotSaveRequested(void);
		void slotTextChanged(void);
		void slotFindRequested(void);
		void slotFindClosing(void);
		void slotDuplicateLine(void);
		void slotCurrentColorStyleChanged(const ot::ColorStyle& _style);
		void slotSelectionChanged(void);

	private:
		friend class TextEditorSearchPopup;

		void getCurrentLineSelection(QList<QTextEdit::ExtraSelection>& _selections);
		void addAdditionalSelections(QList<QTextEdit::ExtraSelection>& _selections);

		TextEditorSearchPopup* m_searchPopup;
		
		bool m_contentChanged;
		std::string m_textEditorName;
		int m_tabSpaces;
		bool m_newLineSamePrefix;
		bool m_enableDuplicateLineShortcut;
		bool m_enableSameTextHighlighting;
		int m_sameTextHighlightingMinimum;

		ot::UID								m_displayedTextEntityID = 0;
		ot::UID								m_displayedTextEntityVersion = 0;

		TextEditorLineNumberArea*			m_lineNumberArea;
		QSyntaxHighlighter *				m_syntaxHighlighter;
	};
}
