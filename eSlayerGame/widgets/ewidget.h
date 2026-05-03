#ifndef EWIDGET_H
#define EWIDGET_H

#include "epainter.h"
#include "emouseevent.h"
#include "../eresolution.h"

#include <vector>
#include <functional>

class eMainWindow;
class eLayout;

using eAction = std::function<void()>;

class eWidget {
public:
    eWidget(eMainWindow* const window);
protected:
    virtual ~eWidget();
public:
    void move(const int x, const int y);
    void setX(const int x);
    void setY(const int y);
    void resize(const int w, const int h);
    void setWidth(const int w);
    void setHeight(const int h);
    void setPadding(const int padding);
    void fitContent();
    void fitWidth();
    void fitHeight();
    void show();
    void hide();
    void setVisible(const bool v);
    void clearWidgetPointers();
    void setTooltip(const std::string& tt);
    const std::string& tooltip() const { return mTooltip; }

    void setHugePadding();
    void setLargePadding();
    void setSmallPadding();
    void setTinyPadding();
    void setVeryTinyPadding();
    void setVeryVeryTinyPadding();
    void setNoPadding();

    void align(const eAlignment a);

    int x() const { return mX; }
    int y() const { return mY; }
    int width() const { return mWidth; }
    int height() const { return mHeight; }
    int padding() const { return mPadding; }
    SDL_Rect rect() const { return {0, 0, mWidth, mHeight}; }
    SDL_Rect globalRect() const;
    bool visible() const { return mVisible; }
    SDL_Point mousePos() const;
    bool hovered() const;

    eWidget* parent() const { return mParent; }

    bool contains(const int x, const int y) const;

    void mapToGlobal(int& x, int& y) const;
    void mapFromGlobal(int& x, int& y) const;
    void mapToParent(int& x, int& y) const;
    void mapFromParent(int& x, int& y) const;
    void mapTo(const eWidget* const to, int& x, int& y) const;
    void mapFrom(const eWidget* const from, int& x, int& y) const;

    void paint(ePainter& p);
    bool keyPress(const eKeyPressEvent& e);
    bool textInput(const eTextInputEvent& e);
    bool mousePress(const eMouseEvent& e);
    bool mouseRelease(const eMouseEvent& e);
    bool mouseMove(const eMouseEvent& e);
    bool mouseWheel(const eMouseWheelEvent& e);

    const std::vector<eWidget*>& children() const { return mChildren; }

    void deleteLater();

    eWidget* lastAncestor();

    void removeChildren();

    static eWidget* sUnderMouse() {
        return sWidgetUnderMouse;
    }
private:
    template <typename T>
    using TMouseEvent = bool (eWidget::*)(const T& e);
    template <typename T>
    eWidget* mouseEvent(const T& e, const TMouseEvent<T> event,
                        const bool overwrite = false);
public:
    void grabMouse();
    bool releaseMouse();
    bool isMouseGrabber();

    void grabKeyboard();
    bool releaseKeyboard();
    bool isKeyboardGrabber();

    bool isLastPressed() const;

    void insertWidget(const int id, eWidget* const w);
    void prependWidget(eWidget* const w);
    void addWidget(eWidget* const w);
    void bringToFront(eWidget* const w);
    void bringToFront();
    void removeWidget(eWidget* const w);
    void removeAllWidgets();

    void stackVertically(const int p = 0,
                         const bool skipHidden = false);
    void layoutVertically(const bool skipHidden = false);
    void layoutVerticallyWithoutSpaces();
    void stackHorizontally(const int p = 0);
    void layoutHorizontally();
    void layoutHorizontallyWithoutSpaces();

    void setMouseReceiver(eWidget* const w);
    void setMouseReceiverDXDY(const int dx, const int dy);

    eMainWindow* window() const { return mWindow; }
protected:
    virtual void sizeHint(int& w, int& h);

    virtual void paintEvent(ePainter& p);

    virtual bool keyPressEvent(const eKeyPressEvent& e) {
        (void)e;
        return false;
    }

    virtual bool textInputEvent(const eTextInputEvent& e) {
        (void)e;
        return false;
    }

    virtual bool mousePressEvent(const eMouseEvent& e) {
        (void)e;
        return false;
    }

    virtual bool mouseReleaseEvent(const eMouseEvent& e) {
        (void)e;
        return false;
    }

    virtual bool mouseEnterEvent(const eMouseEvent& e) {
        (void)e;
        return false;
    }

    virtual bool mouseLeaveEvent(const eMouseEvent& e) {
        (void)e;
        return false;
    }

    virtual bool mouseMoveEvent(const eMouseEvent& e) {
        (void)e;
        return false;
    }

    virtual bool mouseWheelEvent(const eMouseWheelEvent& e) {
        (void)e;
        return false;
    }

    SDL_Renderer* renderer() const;
    const eResolution& resolution() const;
private:
    static eWidget* sWidgetUnderMouse;
    static eWidget* sLastPressed;
    static eWidget* sMouseGrabber;
    static eWidget* sKeyboardGrabber;

    int mX = 0;
    int mY = 0;
    int mWidth = 0;
    int mHeight = 0;
    int mPadding = 0;
    bool mVisible = true;
    bool mDeleteLater = false;
    eWidget* mParent = nullptr;
    std::vector<eWidget*> mChildren;
    eMainWindow* const mWindow;

    std::string mTooltip;

    eWidget* mMouseReceiver = nullptr;
    int mMouseReceiverDx = 0;
    int mMouseReceiverDy = 0;
};

#endif // EWIDGET_H
