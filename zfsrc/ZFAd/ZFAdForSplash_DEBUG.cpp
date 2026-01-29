#include "ZFAd/protocol/ZFProtocolZFAdForSplash.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclass ZFAdForSplashImpl_DEBUG : zfextend ZFObject, zfimplement ZFAdForSplashImpl {
    ZFOBJECT_DECLARE(ZFAdForSplashImpl_DEBUG, ZFObject)
    ZFIMPLEMENT_DECLARE(ZFAdForSplashImpl)

public:
    zfclass Impl : zfextend ZFObject {
        ZFOBJECT_DECLARE(Impl, ZFObject)
    public:
        void *nativeAd(void) {
            return this;
        }
        void start(ZF_IN ZFAdForSplash *ad, ZF_IN ZFUIRootWindow *window) {
            zfobj<ZFTaskQueue> task;

            zfobj<ZFUIWindow> adWindow(window);
            adWindow->viewId("ZFAdForSplashImpl_DEBUG");
            adWindow->bgColor(ZFUIColorRandom(0.9f));
            adWindow->windowLevel(ZFUIWindowLevelOverlay());

            ZFLISTENER_3(onDisplay
                    , zfautoT<ZFUIWindow>, adWindow
                    , zfweakT<ZFAdForSplash>, ad
                    , zfweakT<ZFUIRootWindow>, window
                    ) {
                adWindow->show();
                ZFAdForSplashImpl::implForAd(ad)->notifyAdOnDisplay(ad);
                ZFTask *task = zfargs.sender();
                task->notifySuccess();
            } ZFLISTENER_END()
            task->child(zfobj<ZFWaitTask>(1000));
            task->child(onDisplay);

            ZFLISTENER_2(onStop
                    , zfautoT<ZFUIWindow>, adWindow
                    , zfweakT<ZFAdForSplash>, ad
                    ) {
                adWindow->hide();
                ZFAdForSplashImpl::implForAd(ad)->notifyAdOnStop(ad, v_ZFResultType::e_Success);
                ZFTask *task = zfargs.sender();
                task->notifySuccess();
            } ZFLISTENER_END()
            task->child(zfobj<ZFWaitTask>(4000));
            task->child(onStop);

            task->start();
        }
    };

public:
    zfoverride
    virtual void *nativeAdCreate(
            ZF_IN ZFAdForSplash *ad
            , ZF_IN const zfstring &appId
            , ZF_IN const zfstring &adId
            ) {
        zfobj<Impl> impl;
        ad->objectTag("ZFAdForSplashImpl_DEBUG", impl);
        return impl->nativeAd();
    }
    zfoverride
    virtual void nativeAdDestroy(ZF_IN ZFAdForSplash *ad) {
    }

    zfoverride
    virtual void nativeAdStart(
            ZF_IN ZFAdForSplash *ad
            , ZF_IN ZFUIRootWindow *window
            ) {
        zfautoT<Impl> impl = ad->objectTag("ZFAdForSplashImpl_DEBUG");
        impl->start(ad, window);
    }
};
ZFOBJECT_REGISTER(ZFAdForSplashImpl_DEBUG)

ZF_NAMESPACE_GLOBAL_END

