#include "ZFAdForSplashHelper.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForSplashHelperPrivate {
public:
    zfclassLikePOD Cfg {
    public:
        zfstring implName;
        zfstring appId;
        zfstring adId;
        zfstring systemName;
        zfstring localeId;
        zfstring localeLangId;
    };
public:
    ZFAdForSplashHelper *owner;
    ZFCoreArray<Cfg> cfgList;
    zfautoT<ZFAdForSplash> impl; // current impl, may be null
    zfindex index; // next index to try, may exceed cfgList

    zfobj<ZFObject> observerOwner;
    zfweakT<ZFUIRootWindow> window;
    zftimet startTime; // 0 when not started
    zfbool implShowing;
    zfbool loadingViewShowFlag;
    zfuint loadingViewManualShowFlag;
    zfautoT<ZFUIWindow> loadingViewWindow;
    zfautoT<ZFTimer> loadingViewWindowHideDelay;
    zfauto holder; // retained during running

    zftimet silentDurationBegin; // last time has showed ad
    zfauto attachObserverOwner; // not null if attached
    zfauto attachHolder; // retained during attached

public:
    _ZFP_ZFAdForSplashHelperPrivate(void)
    : cfgList()
    , impl()
    , index(0)
    , observerOwner()
    , window()
    , startTime()
    , implShowing(zffalse)
    , loadingViewShowFlag(zffalse)
    , loadingViewManualShowFlag(0)
    , loadingViewWindow()
    , loadingViewWindowHideDelay()
    , holder()
    , silentDurationBegin(zftimetInvalid())
    , attachObserverOwner()
    , attachHolder()
    {
    }
public:
    void implStop(void) {
        if(this->impl) {
            ZFObserverGroupRemove(this->observerOwner);
            this->impl = zfnull;
        }
        this->implShowing = zffalse;
    }
    void loadingViewUpdate(void) {
        if(this->loadingViewWindowHideDelay) {
            this->loadingViewWindowHideDelay->stop();
            this->loadingViewWindowHideDelay = zfnull;
        }
        if(this->loadingViewShowFlag || this->loadingViewManualShowFlag > 0) {
            if(this->loadingViewWindow == zfnull) {
                this->_loadingViewShowImpl();
            }
        }
        else {
            if(this->loadingViewWindow != zfnull) {
                this->_loadingViewHideImpl();
            }
        }
    }
private:
    void _loadingViewShowImpl(void) {
        this->loadingViewWindow = zfobj<ZFUIWindow>(owner->window());
        this->loadingViewWindow->viewId("ZFAdForSplashLoadingView");
        this->loadingViewWindow->bgColor(ZFUIColorCreateRGB(0x000000));
        this->loadingViewWindow->windowLevel(ZFUIWindowLevelOverlay() - 1);
        this->loadingViewWindow->windowMarginShouldApply(zffalse);
        if(owner->loadingView()) {
            owner->loadingView()->removeFromParent();
            this->loadingViewWindow->child(owner->loadingView())->sizeFill();
        }
        this->loadingViewWindow->show();
    }
    void _loadingViewHideImpl(void) {
        if(this->loadingViewWindowHideDelay) {
            this->loadingViewWindowHideDelay->stop();
            this->loadingViewWindowHideDelay = zfnull;
        }
        ZFLISTENER_1(loadingViewWindowOnDelayHide
                , zfweakT<ZFAdForSplashHelper>, owner
                ) {
            if(owner->loadingView()) {
                owner->loadingView()->removeFromParent();
            }
            if(owner->d->loadingViewWindow) {
                owner->d->loadingViewWindow->hide();
                owner->d->loadingViewWindow = zfnull;
            }
        } ZFLISTENER_END()
        this->loadingViewWindowHideDelay = ZFTimerOnce(500, loadingViewWindowOnDelayHide);
    }
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForSplashHelper)

ZFMETHOD_DEFINE_1(ZFAdForSplashHelper, zfautoT<ZFAdForSplashHelper>, instance
        , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
        ) {
    if(window == zfnull) {
        window = ZFUIRootWindow::mainWindow();
    }
    zfautoT<ZFAdForSplashHelper> ret = window->objectTag("_ZFP_ZFAdForSplashHelper_instance");
    if(!ret) {
        ret = zfobj<ZFAdForSplashHelper>();
        ret->d->window = window;
        window->objectTag("_ZFP_ZFAdForSplashHelper_instance", ret);
    }
    return ret;
}

ZFMETHOD_DEFINE_6(ZFAdForSplashHelper, ZFAdForSplashHelper *, cfg
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        , ZFMP_IN_OPT(const zfstring &, systemName, zfnull)
        , ZFMP_IN_OPT(const zfstring &, localeId, zfnull)
        , ZFMP_IN_OPT(const zfstring &, localeLangId, zfnull)
        ) {
    if(zffalse
            || !implName
            || !appId
            || !adId
            ) {
        return this;
    }
    _ZFP_ZFAdForSplashHelperPrivate::Cfg cfg;
    cfg.implName = implName;
    cfg.appId = appId;
    cfg.adId = adId;
    cfg.systemName = systemName;
    cfg.localeId = localeId;
    cfg.localeLangId = localeLangId;
    d->cfgList.add(cfg);
    return this;
}

ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfanyT<ZFUIRootWindow>, window) {
    return d->window ? d->window.get() : ZFUIRootWindow::mainWindow();
}

ZFMETHOD_DEFINE_1(ZFAdForSplashHelper, void, start
        , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
        ) {
    if(this->started()) {
        return;
    }

    if(this->skipped()) {
        return;
    }

    d->holder = this;
    d->loadingViewShowFlag = zftrue;
    d->loadingViewUpdate();

    d->startTime = ZFTime::currentTime();
    this->observerNotify(ZFAdForSplash::E_AdOnStart(), this->window());

    zfclassNotPOD _Impl {
    public:
        static void tryNext(ZF_IN ZFAdForSplashHelper *owner) {
            if(owner->d->impl || owner->d->index >= owner->d->cfgList.count()) {
                _stop(owner, zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("no valid impl"));
                return;
            }
            if(ZFTime::currentTime() - owner->d->startTime >= owner->timeout()) {
                _stop(owner, zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("load timeout"));
                return;
            }

            _ZFP_ZFAdForSplashHelperPrivate::Cfg const &cfg = owner->d->cfgList[(owner->d->index)++];
            zfbool valid = zffalse;
            do {
                if(cfg.systemName) {
                    zfstring systemName = ZFEnvInfo::systemName();
                    if(systemName && !ZFRegExpMatch(systemName, cfg.systemName)) {
                        break;
                    }
                }

                if(cfg.localeId) {
                    zfstring localeId = ZFEnvInfo::localeId();
                    if(localeId && !ZFRegExpMatch(localeId, cfg.localeId)) {
                        break;
                    }
                }

                if(cfg.localeLangId) {
                    zfstring localeLangId = ZFEnvInfo::localeLangId();
                    if(localeLangId && !ZFRegExpMatch(localeLangId, cfg.localeLangId)) {
                        break;
                    }
                }

                valid = zftrue;
            } while(zffalse);
            if(!valid) {
                tryNext(owner);
                return;
            }

            zfobj<ZFAdForSplash> impl;
            owner->d->impl = impl;

            zfweakT<ZFAdForSplashHelper> weakOwner = owner;

            ZFLISTENER_1(AdOnError
                    , zfweakT<ZFAdForSplashHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->d->implShowing = zffalse;
                weakOwner->d->implStop();
                tryNext(weakOwner);
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnDisplay
                    , zfweakT<ZFAdForSplashHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->d->implShowing = zftrue;
                weakOwner->d->loadingViewShowFlag = zffalse;
                weakOwner->d->loadingViewUpdate();
                weakOwner->observerNotify(ZFAdForSplash::E_AdOnDisplay(), zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnClick
                    , zfweakT<ZFAdForSplashHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->observerNotify(ZFAdForSplash::E_AdOnClick(), zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnStop
                    , zfweakT<ZFAdForSplashHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->d->implShowing = zffalse;
                _stop(weakOwner, zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFObserverGroup(owner->d->observerOwner, impl)
                .observerAdd(ZFAdForSplash::E_AdOnError(), AdOnError)
                .observerAdd(ZFAdForSplash::E_AdOnDisplay(), AdOnDisplay)
                .observerAdd(ZFAdForSplash::E_AdOnClick(), AdOnClick)
                .observerAdd(ZFAdForSplash::E_AdOnStop(), AdOnStop)
                ;

            impl->setup(cfg.implName, cfg.appId, cfg.adId);
            impl->start();
        }
    private:
        static void _stop(
                ZF_IN ZFAdForSplashHelper *owner
                , ZF_IN v_ZFResultType *resultType
                , ZF_IN v_zfstring *errorHint
                ) {
            owner->d->index = 0;
            owner->d->startTime = 0;
            owner->d->implStop();
            owner->d->loadingViewShowFlag = zffalse;
            owner->d->loadingViewUpdate();
            if(resultType->zfv() == v_ZFResultType::e_Fail) {
                owner->observerNotify(ZFAdForSplash::E_AdOnError(), errorHint);
            }
            owner->observerNotify(ZFAdForSplash::E_AdOnStop(), resultType, errorHint);
            owner->d->holder = zfnull;
        }
    };
    _Impl::tryNext(this);
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfbool, started) {
    return d->startTime != 0;
}

void ZFAdForSplashHelper::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForSplashHelperPrivate);
    d->owner = this;

    zfindex count = zfindexMax();
    zfstring countStr = ZFState::instance()->get("ZFAdForSplashHelper_skipCount");
    if(countStr) {
        zfindexFromStringT(count, countStr);
    }
    ZFState::instance()->set("ZFAdForSplashHelper_skipCount", zfindexToString(++count));
}
void ZFAdForSplashHelper::objectOnDealloc(void) {
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}

// ============================================================
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, void, attach) {
    zfobjReleaseInScope(zfobjRetain(this));
    this->detach();

    d->attachObserverOwner = zfobj<ZFObject>();
    d->attachHolder = this;
    zfself *owner = this;
    ZFLISTENER_1(windowOnShow
            , zfweakT<zfself>, owner
            ) {
        zftimet curTime = ZFTime::currentTime();
        if(!owner->d->implShowing && (zffalse
                    || owner->d->silentDurationBegin == zftimetInvalid()
                    || curTime - owner->d->silentDurationBegin > owner->silentDuration()
                    )) {
            owner->d->silentDurationBegin = curTime;
            owner->start();
        }
    } ZFLISTENER_END()
    ZFObserverGroup(d->attachObserverOwner, this->window())
        .observerAdd(ZFUIRootWindow::E_WindowOnResume(), windowOnShow)
        ;
    if(this->window()->windowResumed()) {
        windowOnShow.execute();
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, void, detach) {
    if(d->attachObserverOwner) {
        ZFObserverGroupRemove(d->attachObserverOwner);
        d->attachObserverOwner = zfnull;
        d->attachHolder = zfnull;
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfbool, attached) {
    return d->attachObserverOwner;
}

// ============================================================
ZFMETHOD_DEFINE_2(ZFAdForSplashHelper, void, loadingIcon
        , ZFMP_IN(ZFUIImage *, icon)
        , ZFMP_IN_OPT(ZFUIColor, bgColor, ZFUIColorCreateRGB(0x000000))
        ) {
    zfobj<ZFUIView> container;
    container->bgColor(bgColor);
    this->loadingView(container);

    zfobj<ZFUIImageView> view;
    container->child(view)->alignCenter();
    view->image(icon);

    zfobj<ZFAniQueue> ani;
    ZFLISTENER_1(onUpdate
            , zfautoT<ZFAnimation>, ani
            ) {
        ZFUIView *owner = zfargs.sender();
        if(owner->viewTreeInWindow()) {
            if(!ani->started()) {
                ani->target(owner);
                ani->start();
            }
        }
        else {
            if(ani->started()) {
                ani->stop();
            }
        }
    } ZFLISTENER_END()
    view->observerAdd(ZFUIView::E_ViewTreeInWindowOnUpdate(), onUpdate);

    zftimet dOffset = 800;
    zftimet dScale = 80;
    zffloat offset = 48;
    zffloat scale = 0.2f;
    ani->loop(zfindexMax());
    ani
        ->c_child(ZFAni("translateY", zfobj<v_zffloat>(0), zfobj<v_zffloat>(-offset))->c_curve(zfobj<ZFCurveEaseOut>())->c_duration(dOffset))
        ->c_child(ZFAni("translateY", zfobj<v_zffloat>(-offset), zfobj<v_zffloat>(0))->c_curve(zfobj<ZFCurveEaseIn>())->c_duration(dOffset))
        ->c_child(zfobj<ZFAniGroup>()
                ->c_child(ZFAni("scaleX", zfobj<v_zffloat>(1), zfobj<v_zffloat>(1 + scale)))
                ->c_child(ZFAni("scaleY", zfobj<v_zffloat>(1), zfobj<v_zffloat>(1 - scale)))
                ->c_duration(dScale)
                )
        ->c_child(zfobj<ZFAniGroup>()
                ->c_child(ZFAni("scaleX", zfobj<v_zffloat>(1 + scale), zfobj<v_zffloat>(1)))
                ->c_child(ZFAni("scaleY", zfobj<v_zffloat>(1 - scale), zfobj<v_zffloat>(1)))
                ->c_duration(dScale)
                )
        ;
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, void, loadingViewShow) {
    ++(d->loadingViewManualShowFlag);
    d->loadingViewUpdate();
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, void, loadingViewHide) {
    --(d->loadingViewManualShowFlag);
    d->loadingViewUpdate();
}

ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfbool, skipped) {
    if(this->skipCount() != 0) {
        zfindex count = zfindexMax();
        zfstring countStr = ZFState::instance()->get("ZFAdForSplashHelper_skipCount");
        if(countStr) {
            zfindexFromStringT(count, countStr);
        }
        return (count < this->skipCount());
    }
    else {
        return zffalse;
    }
}

// ============================================================
ZFMETHOD_FUNC_DEFINE_2(zfautoT<ZFTask>, ZFAdForSplashTask
        , ZFMP_IN_OPT(ZFAdForSplashHelper *, ad, zfnull)
        , ZFMP_IN_OPT(zfbool, finishWhenDisplay, zftrue)
        ) {
    if(!ad) {
        ad = ZFAdForSplashHelper::instance();
    }
    zfobj<ZFObject> observerHolder;

    ZFLISTENER_3(onStart
            , zfauto, observerHolder
            , zfautoT<ZFAdForSplashHelper>, ad
            , zfbool, finishWhenDisplay
            ) {
        ZFTask *ownerTask = zfargs.sender();
        ad->attach();
        if(ad->window()->windowResumed() && !ad->started()) {
            ownerTask->notifySuccess();
            return;
        }
        ZFLISTENER_1(adOnStop
                , zfweakT<ZFTask>, ownerTask
                ) {
            if(ownerTask) {
                ownerTask->notifySuccess();
            }
        } ZFLISTENER_END()
        ZFObserverGroup(observerHolder, ad)
            .observerAdd(ZFAdForSplash::E_AdOnDisplay(), finishWhenDisplay ? adOnStop : ZFCallback())
            .observerAdd(ZFAdForSplash::E_AdOnError(), adOnStop)
            .observerAdd(ZFAdForSplash::E_AdOnStop(), adOnStop)
            ;

        if(finishWhenDisplay) {
            ZFTimerOnce(3000, adOnStop);
        }
    } ZFLISTENER_END()

    ZFLISTENER_1(onStop
            , zfauto, observerHolder
            ) {
        ZFObserverGroupRemove(observerHolder);
    } ZFLISTENER_END()

    zfobj<ZFTask> task;
    task->observerAdd(ZFTask::E_TaskOnStart(), onStart);
    task->observerAdd(ZFTask::E_TaskOnStop(), onStop);
    return task;
}
ZFMETHOD_FUNC_DEFINE_1(zfautoT<ZFTask>, ZFAdForSplashLoadingShowTask
        , ZFMP_IN_OPT(ZFAdForSplashHelper *, ad, zfnull)
        ) {
    if(!ad) {
        ad = ZFAdForSplashHelper::instance();
    }
    ZFLISTENER_1(onStart
            , zfautoT<ZFAdForSplashHelper>, ad
            ) {
        ad->loadingViewShow();
        ZFTask *ownerTask = zfargs.sender();
        ownerTask->notifySuccess();
    } ZFLISTENER_END()
    zfobj<ZFTask> task;
    task->observerAdd(ZFTask::E_TaskOnStart(), onStart);
    return task;
}
ZFMETHOD_FUNC_DEFINE_1(zfautoT<ZFTask>, ZFAdForSplashLoadingHideTask
        , ZFMP_IN_OPT(ZFAdForSplashHelper *, ad, zfnull)
        ) {
    if(!ad) {
        ad = ZFAdForSplashHelper::instance();
    }
    ZFLISTENER_1(onStart
            , zfautoT<ZFAdForSplashHelper>, ad
            ) {
        ad->loadingViewHide();
        ZFTask *ownerTask = zfargs.sender();
        ownerTask->notifySuccess();
    } ZFLISTENER_END()
    zfobj<ZFTask> task;
    task->observerAdd(ZFTask::E_TaskOnStart(), onStart);
    return task;
}

ZF_NAMESPACE_GLOBAL_END

