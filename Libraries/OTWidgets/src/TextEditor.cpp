//! @file TextEditor.cpp
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TextEditor.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextdocument.h>
#include <QtGui/qtextlist.h>
#include <QtWidgets/qshortcut.h>

/*

ot::TextEditorSyntaxHighlighterTest::TextEditorSyntaxHighlighterTest(TextEditor * _editor)
	: QSyntaxHighlighter(_editor->document())
{
	HighlightingRule rule;

	m_keywordFormat.setForeground(Qt::GlobalColor::blue);
	m_keywordFormat.setFontWeight(QFont::Bold);
	const QString keywordPatterns[] = {
		QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
		QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
		QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
		QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
		QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
		QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
		QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
		QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
		QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
		QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b")
	};
	for (const QString &pattern : keywordPatterns) {
		rule.pattern = QRegularExpression(pattern);
		rule.format = m_keywordFormat;
		m_highlightingRules.append(rule);
	}

	m_classFormat.setFontWeight(QFont::Bold);
	m_classFormat.setForeground(Qt::darkMagenta);
	rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
	rule.format = m_classFormat;
	m_highlightingRules.append(rule);

	m_quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
	rule.format = m_quotationFormat;
	m_highlightingRules.append(rule);

	m_functionFormat.setFontItalic(true);
	m_functionFormat.setForeground(Qt::blue);
	rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
	rule.format = m_functionFormat;
	m_highlightingRules.append(rule);

	m_singleLineCommentFormat.setForeground(Qt::green);
	rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
	rule.format = m_singleLineCommentFormat;
	m_highlightingRules.append(rule);

	m_multiLineCommentFormat.setForeground(Qt::green);
	m_commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
	m_commentEndExpression = QRegularExpression(QStringLiteral("\\* /"));  <- no space
}

*/

ot::TextEditorLineNumberArea::TextEditorLineNumberArea(TextEditor* _editor)
	: QWidget(_editor), m_editor(_editor)
{}

ot::TextEditorLineNumberArea::~TextEditorLineNumberArea() {}

QSize ot::TextEditorLineNumberArea::sizeHint() const {
	return QSize(m_editor->lineNumberAreaWidth(), 0);
}

void ot::TextEditorLineNumberArea::paintEvent(QPaintEvent * _event) {
	m_editor->lineNumberAreaPaintEvent(_event);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ot::TextEditor::TextEditor(QWidget* _parent) : PlainTextEdit(_parent), m_syntaxHighlighter(nullptr), m_contentChanged(false) {

	m_lineNumberArea = new TextEditorLineNumberArea(this);

	QFont f = font();
	f.setFixedPitch(true);
	f.setFamily("Consolas");
	setFont(f);
	QFontMetrics fm(f);
	this->setTabStopDistance(4);

	m_saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
	m_saveShortcut->setContext(Qt::WidgetShortcut);

	connect(this, &TextEditor::blockCountChanged, this, &TextEditor::slotUpdateLineNumberAreaWidth);
	connect(this, &TextEditor::updateRequest, this, &TextEditor::slotUpdateLineNumberArea);
	connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::slotHighlightCurrentLine);
	connect(this, &TextEditor::textChanged, this, &TextEditor::slotTextChanged);
	connect(m_saveShortcut, &QShortcut::activated, this, &TextEditor::slotSaveRequested);
}

ot::TextEditor::~TextEditor() {

}

int ot::TextEditor::lineNumberAreaWidth(void) const {
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}
	digits += 2;

	int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

	return space;
}

void ot::TextEditor::lineNumberAreaPaintEvent(QPaintEvent * _event) {
	QPainter painter(m_lineNumberArea);
	QColor colorBack(Qt::white);
	QColor colorFront(Qt::black);
	QColor colorSmooth(Qt::lightGray);
	painter.fillRect(_event->rect(), colorBack);
	painter.setPen(colorSmooth);
	painter.drawLine(_event->rect().x() + _event->rect().width() - 1, _event->rect().y(),
		_event->rect().x() + _event->rect().width() - 1, _event->rect().y() + _event->rect().height());

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + qRound(blockBoundingRect(block).height());
	int ptSize = font().pointSize() - 2;

	while (block.isValid() && top <= _event->rect().bottom()) {
		if (block.isVisible() && bottom >= _event->rect().top()) {
			QString number = QString::number(blockNumber + 1) + "  ";
			painter.setPen(colorFront);
			QFont f = painter.font();
			f.setPointSize(ptSize);
			painter.setFont(f);
			painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(),
				Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + qRound(blockBoundingRect(block).height());
		++blockNumber;
	}
}

void ot::TextEditor::setCode(const QString& _text) {
	setPlainText(_text);
	document()->clearUndoRedoStacks();
}

void ot::TextEditor::setCode(const QStringList& _lines) {
	clear();
	for (auto l : _lines) appendPlainText(l);
	document()->clearUndoRedoStacks();
}

QStringList ot::TextEditor::code(void) const {
	return toPlainText().split("\n", Qt::KeepEmptyParts);
}

void ot::TextEditor::setSyntaxHighlighter(QSyntaxHighlighter * _highlighter) {
	if (m_syntaxHighlighter) delete m_syntaxHighlighter;
	m_syntaxHighlighter = _highlighter;
}

void ot::TextEditor::resizeEvent(QResizeEvent * _event) {
	PlainTextEdit::resizeEvent(_event);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void ot::TextEditor::slotUpdateLineNumberAreaWidth(int _newBlockCount) {
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ot::TextEditor::slotHighlightCurrentLine() {
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void ot::TextEditor::slotUpdateLineNumberArea(const QRect & _rect, int _dy) {
	if (_dy) m_lineNumberArea->scroll(0, _dy);
	else m_lineNumberArea->update(0, _rect.y(), m_lineNumberArea->width(), _rect.height());

	if (_rect.contains(viewport()->rect())) slotUpdateLineNumberAreaWidth(0);
}

void ot::TextEditor::slotSaveRequested(void) {
	if (!m_contentChanged) return;
	emit saveRequested();
}

void ot::TextEditor::slotTextChanged(void) {
	m_contentChanged = true;
}