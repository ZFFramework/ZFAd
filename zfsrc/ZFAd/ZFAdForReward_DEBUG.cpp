#include "ZFAd/protocol/ZFProtocolZFAdForReward.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclass ZFAdForRewardImpl_DEBUG : zfextend ZFObject, zfimplement ZFAdForRewardImpl {
    ZFOBJECT_DECLARE(ZFAdForRewardImpl_DEBUG, ZFObject)
    ZFIMPLEMENT_DECLARE(ZFAdForRewardImpl)

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
        void start(ZF_IN ZFAdForReward *ad) {
            zfobj<ZFTaskQueue> task;

            zfobj<ZFUIWindow> adWindow(ad->window());
            adWindow->viewId("ZFAdForRewardImpl_DEBUG");
            adWindow->bgColor(ZFUIColorRandom(0.9f));
            adWindow->windowLevel(ZFUIWindowLevelOverlay());

            ZFLISTENER_2(onDisplay
                    , zfautoT<ZFUIWindow>, adWindow
                    , zfweakT<ZFAdForReward>, ad
                    ) {
                adWindow->show();
                ZFAdForRewardImpl::implForAd(ad)->notifyAdOnDisplay(ad);
                ZFTask *task = zfargs.sender();
                task->notifySuccess();
            } ZFLISTENER_END()
            task->child(zfobj<ZFWaitTask>(500));
            task->child(onDisplay);

            ZFLISTENER_2(onStop
                    , zfautoT<ZFUIWindow>, adWindow
                    , zfweakT<ZFAdForReward>, ad
                    ) {
                adWindow->hide();
                ZFAdForRewardImpl::implForAd(ad)->notifyAdOnStop(ad, v_ZFResultType::e_Success);
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
            ZF_IN ZFAdForReward *ad
            , ZF_IN const zfstring &appId
            , ZF_IN const zfstring &adId
            ) {
        zfobj<Impl> impl;
        ad->objectTag("ZFAdForRewardImpl_DEBUG", impl);
        return impl->nativeAd();
    }
    zfoverride
    virtual void nativeAdDestroy(ZF_IN ZFAdForReward *ad) {
    }

    zfoverride
    virtual void nativeAdLoad(ZF_IN ZFAdForReward *ad) {
        Impl *impl = (Impl *)ad->nativeAd();
        if(impl->loadTaskId) {
            impl->loadTaskId->stop();
        }
        ZFLISTENER_1(onLoad
                , zfweakT<ZFAdForReward>, ad
                ) {
            Impl *impl = (Impl *)ad->nativeAd();
            impl->loadTaskId = zfnull;
            impl->loadedTime = ZFTime::currentTime();
            ZFAdForRewardImpl::implForAd(ad)->notifyAdOnLoad(ad);
        } ZFLISTENER_END()
        impl->loadTaskId = ZFTimerOnce(1500, onLoad);
    }
    zfoverride
    virtual zfbool nativeAdLoaded(ZF_IN ZFAdForReward *ad) {
        Impl *impl = (Impl *)ad->nativeAd();
        return impl->loadedTime != zftimetInvalid()
            && ZFTime::currentTime() - impl->loadedTime < 5 * zftimetOneMinute()
            ;
    }

    zfoverride
    virtual void nativeAdStart(ZF_IN ZFAdForReward *ad) {
        zfautoT<Impl> impl = ad->objectTag("ZFAdForRewardImpl_DEBUG");
        impl->start(ad);
    }
};
ZFOBJECT_REGISTER(ZFAdForRewardImpl_DEBUG)

ZF_NAMESPACE_GLOBAL_END

