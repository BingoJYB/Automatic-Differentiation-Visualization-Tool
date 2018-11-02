#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;

class CodeViewer : public QPlainTextEdit {
    Q_OBJECT

public:
    CodeViewer(QWidget *parent = 0);

    void mouseReleaseEvent(QMouseEvent *event);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

signals:
    void callSetHighlightScene2(int id) const;

protected:
   void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
   void send(int id) const;

    QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget {
public:
    LineNumberArea(CodeViewer *viewer) : QWidget(viewer) {
        codeViewer = viewer;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(codeViewer->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeViewer->lineNumberAreaPaintEvent(event);
    }

private:
    CodeViewer *codeViewer;
};

#endif
