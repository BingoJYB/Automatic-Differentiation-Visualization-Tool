#include <QtWidgets>

#include "global.h"
#include "linenumber.h"

CodeViewer::CodeViewer(QWidget *parent)
    : QPlainTextEdit(parent) {
    lineNumberArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeViewer::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeViewer::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeViewer::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CodeViewer::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeViewer::updateLineNumberAreaWidth(int) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeViewer::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeViewer::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeViewer::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    setExtraSelections(extraSelections);
}

void CodeViewer::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeViewer::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        int lineNum = editor->textCursor().blockNumber() + 1;
        int columnNum = editor->textCursor().positionInBlock();
        long id = 0;

        for (unsigned i = 0; i < g_mapping_helper._positions.size(); i++) {
            if (g_mapping_helper._positions.at(i).line == lineNum && columnNum >= g_mapping_helper._positions.at(i).col) {
                if (i == g_mapping_helper._positions.size()-1
                        || (i != g_mapping_helper._positions.size()-1 && g_mapping_helper._positions.at(i+1).line != lineNum)
                        || (i != g_mapping_helper._positions.size()-1 && g_mapping_helper._positions.at(i+1).line == lineNum
                        && columnNum < g_mapping_helper._positions.at(i+1).col)) {
                    mainwindow->setHighlightCode(i+1);
                    id = i+1;
                    break;
                }
            }
        }

        connect(this, &CodeViewer::callSetHighlightScene2, scene, &GraphicsScene::setHighlightScene2);
        this->send(id);
    }

    QPlainTextEdit::mouseReleaseEvent(event);
}

void CodeViewer::send(int id) const {
    emit callSetHighlightScene2(id);
}
