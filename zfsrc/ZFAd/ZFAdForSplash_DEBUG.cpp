#include "ZFAd/protocol/ZFProtocolZFAdForSplash.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclass ZFAdForSplashImpl_DEBUG : zfextend ZFObject, zfimplement ZFAdForSplashImpl {
    ZFOBJECT_DECLARE(ZFAdForSplashImpl_DEBUG, ZFObject)
    ZFIMPLEMENT_DECLARE(ZFAdForSplashImpl)

public:
    zfclass Impl : zfextend ZFObject {
        ZFOBJECT_DECLARE(Impl, ZFObject)
    public:
        zftimet loadedTime; // zftimetInvalid if not loaded
        zfautoT<ZFTaskId> loadTaskId;
    protected:
        zfoverride
        virtual void objectOnInit(void) {
            zfsuper::objectOnInit();
            this->loadedTime = zftimetInvalid();
        }
        zfoverride
        virtual void objectOnDeallocPrepare(void) {
            if(this->loadTaskId) {
                this->loadTaskId->stop();
                this->loadTaskId = zfnull;
            }
            zfsuper::objectOnDeallocPrepare();
        }
    public:
        void *nativeAd(void) {
            return this;
        }
        void start(ZF_IN ZFAdForSplash *ad) {
            zfobj<ZFTaskQueue> task;

            zfobj<ZFUIWindow> adWindow(ad->window());
            adWindow->viewId("ZFAdForSplashImpl_DEBUG");
            adWindow->bgColor(ZFUIColorRandom(0.9f));
            adWindow->windowLevel(ZFUIWindowLevelOverlay());

            ZFLISTENER_2(onDisplay
                    , zfautoT<ZFUIWindow>, adWindow
                    , zfweakT<ZFAdForSplash>, ad
                    ) {
                adWindow->show();
                ZFAdForSplashImpl::implForAd(ad)->notifyAdOnDisplay(ad);
                ZFTask *task = zfargs.sender();
                task->notifySuccess();
            } ZFLISTENER_END()
            task->child(zfobj<ZFWaitTask>(500));
            task->child(onDisplay);

            ZFLISTENER_2(onStop
                    , zfautoT<ZFUIWindow>, adWindow
                    , zfweakT<ZFAdForSplash>, ad
                    ) {
                adWindow->hide();
                ZFAdForSplashImpl::implForAd(ad)->notifyAdOnStop(ad, v_ZFResultType::e_Success, zfnull);
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
    virtual void nativeAdLoad(ZF_IN ZFAdForSplash *ad) {
        Impl *impl = (Impl *)ad->nativeAd();
        if(impl->loadTaskId) {
            impl->loadTaskId->stop();
        }
        ZFLISTENER_1(onLoad
                , zfweakT<ZFAdForSplash>, ad
                ) {
            Impl *impl = (Impl *)ad->nativeAd();
            impl->loadTaskId = zfnull;
            impl->loadedTime = ZFTime::currentTime();
            ZFAdForSplashImpl::implForAd(ad)->notifyAdOnLoadStop(ad, v_ZFResultType::e_Success, zfnull);
        } ZFLISTENER_END()
        impl->loadTaskId = ZFTimerOnce(1500, onLoad);
    }
    zfoverride
    virtual zfbool nativeAdLoaded(ZF_IN ZFAdForSplash *ad) {
        Impl *impl = (Impl *)ad->nativeAd();
        return impl->loadedTime != zftimetInvalid()
            && ZFTime::currentTime() - impl->loadedTime < 5 * zftimetOneMinute()
            ;
    }

    zfoverride
    virtual void nativeAdStart(ZF_IN ZFAdForSplash *ad) {
        zfautoT<Impl> impl = ad->objectTag("ZFAdForSplashImpl_DEBUG");
        impl->start(ad);
    }
};
ZFOBJECT_REGISTER(ZFAdForSplashImpl_DEBUG)

ZF_NAMESPACE_GLOBAL_END

