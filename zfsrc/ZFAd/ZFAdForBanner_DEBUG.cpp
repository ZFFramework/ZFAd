#include "ZFAd/protocol/ZFProtocolZFAdForBanner.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclass ZFAdForBannerImpl_DEBUG : zfextend ZFObject, zfimplement ZFAdForBannerImpl {
    ZFOBJECT_DECLARE(ZFAdForBannerImpl_DEBUG, ZFObject)
    ZFIMPLEMENT_DECLARE(ZFAdForBannerImpl)

public:
    zfclass Impl : zfextend ZFObject {
        ZFOBJECT_DECLARE(Impl, ZFObject)
    public:
        void *nativeAd(void) {
            return _view->nativeView();
        }
        void attach(ZF_IN ZFAdForBanner *ad) {
            _view = zfobj<ZFUIView>();
            _view->bgColor(ZFUIColorRandom(0.9f));
            ZFLISTENER_1(viewOnEvent
                    , zfweakT<ZFAdForBanner>, ad
                    ) {
                ZFUIView *view = zfargs.sender();
                ZFUIMouseEvent *event = zfargs.param0();
                if(event == zfnull
                        || event->mouseAction != v_ZFUIMouseAction::e_Up
                        || !ZFUIRectIsContainPoint(ZFUIRectGetBounds(view->viewFrame()), event->mousePoint)
                        ) {
                    return;
                }
                ZFAdForBannerImpl::implForAd(ad)->notifyAdOnClose(ad);
            } ZFLISTENER_END()
            _view->observerAdd(ZFUIView::E_ViewOnEvent(), viewOnEvent);

            zfweakT<zfself> owner = this;
            ZFLISTENER_1(viewTreeInWindowOnUpdate
                    , zfweakT<zfself>, owner
                    ) {
                zfweakT<ZFAdForBanner> ad = zfargs.sender();
                if(ad->viewTreeInWindow()) {
                    if(owner->_displayDelay) {
                        owner->_displayDelay->stop();
                    }
                    ZFLISTENER_2(onDisplay
                            , zfweakT<zfself>, owner
                            , zfweakT<ZFAdForBanner>, ad
                            ) {
                        owner->_displayDelay = zfnull;
                        ZFAdForBannerImpl::implForAd(ad)->notifyAdOnDisplay(ad);
                    } ZFLISTENER_END()
                    owner->_displayDelay = ZFTimerOnce(1000, onDisplay);
                }
                else {
                    if(owner->_displayDelay) {
                        owner->_displayDelay->stop();
                        owner->_displayDelay = zfnull;
                    }
                }
            } ZFLISTENER_END()
            ZFObserverGroup(this, ad)
                .observerAdd(ZFUIView::E_ViewTreeInWindowOnUpdate(), viewTreeInWindowOnUpdate)
                ;
        }
        void detach(void) {
            ZFObserverGroupRemove(this);
            _view = zfnull;
        }
    private:
        zfautoT<ZFUIView> _view;
        zfautoT<ZFTaskId> _displayDelay;
    };

public:
    zfoverride
    virtual void *nativeAdCreate(
                ZF_IN ZFAdForBanner *ad
                , ZF_IN const zfstring &appId
                , ZF_IN const zfstring &adId
                ) {
        zfobj<Impl> impl;
        impl->attach(ad);
        ad->objectTag("ZFAdForBannerImpl_DEBUG", impl);
        return impl->nativeAd();
    }
    zfoverride
    virtual void nativeAdDestroy(ZF_IN ZFAdForBanner *ad) {
        zfautoT<Impl> impl = ad->objectTagRemoveAndGet("ZFAdForBannerImpl_DEBUG");
        impl->detach();
    }

    zfoverride
    virtual ZFUISize nativeAdMeasure(
                ZF_IN ZFAdForBanner *ad
                , ZF_IN const ZFUISize &sizeHint
                ) {
        zffloat widthHint = sizeHint.width;
        if(widthHint <= 0) {
            ZFUIView *v = ZFUIRootWindow::mainWindow()->rootView();
            widthHint = v->width() * v->UIScaleFixed();
            if(widthHint <= 0) {
                widthHint = 240;
            }
        }
        return ZFUISizeCreate(sizeHint.width, sizeHint.width * 60 / 240);
    }
};
ZFOBJECT_REGISTER(ZFAdForBannerImpl_DEBUG)

ZF_NAMESPACE_GLOBAL_END

