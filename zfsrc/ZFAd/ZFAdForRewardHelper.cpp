#include "ZFAdForRewardHelper.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForRewardHelperPrivate {
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
    ZFCoreArray<Cfg> cfgList;
    zfautoT<ZFAdForReward> impl; // current impl, may be null
    zfindex index; // next index to try, may exceed cfgList
    ZFCoreArray<ZFListener> onLoadStopList;
    zftimet loadStartTime; // zftimetInvalid when not loading

    zfweakT<ZFUIRootWindow> window;
    zfobj<ZFObject> observerOwner;
    zfbool started;
    zfbool showing;

public:
    _ZFP_ZFAdForRewardHelperPrivate(void)
    : cfgList()
    , impl()
    , index(0)
    , onLoadStopList()
    , loadStartTime(zftimetInvalid())
    , window()
    , observerOwner()
    , started(zffalse)
    , showing(zffalse)
    {
    }
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForRewardHelper)

ZFEVENT_REGISTER(ZFAdForRewardHelper, AdOnCheck)

ZFMETHOD_DEFINE_1(ZFAdForRewardHelper, zfautoT<ZFAdForRewardHelper>, instance
        , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
        ) {
    if(window == zfnull) {
        window = ZFUIRootWindow::mainWindow();
    }
    zfautoT<ZFAdForRewardHelper> ret = window->objectTag("_ZFP_ZFAdForRewardHelper_instance");
    if(!ret) {
        ret = zfobj<ZFAdForRewardHelper>();
        ret->d->window = window;
        window->objectTag("_ZFP_ZFAdForRewardHelper_instance", ret);
    }
    return ret;
}

ZFMETHOD_DEFINE_6(ZFAdForRewardHelper, ZFAdForRewardHelper *, cfg
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
    _ZFP_ZFAdForRewardHelperPrivate::Cfg cfg;
    cfg.implName = implName;
    cfg.appId = appId;
    cfg.adId = adId;
    cfg.systemName = systemName;
    cfg.localeId = localeId;
    cfg.localeLangId = localeLangId;
    d->cfgList.add(cfg);
    return this;
}

ZFMETHOD_DEFINE_0(ZFAdForRewardHelper, zfanyT<ZFUIRootWindow>, window) {
    return d->window ? d->window.get() : ZFUIRootWindow::mainWindow();
}

ZFMETHOD_DEFINE_1(ZFAdForRewardHelper, zfautoT<ZFTaskId>, load
        , ZFMP_IN_OPT(const ZFListener &, onLoadStop, zfnull)
        ) {
    if(d->loadStartTime == zftimetInvalid() && this->loaded()) {
        if(onLoadStop) {
            onLoadStop.execute(ZFArgs()
                    .sender(this)
                    .param0(zfobj<v_ZFResultType>(v_ZFResultType::e_Success))
                    .param1(zfobj<v_zfstring>())
                    );
        }
        return zfnull;
    }
    zfweakT<zfself> owner = this;
    zfobj<ZFTaskIdBasic> taskId;
    ZFLISTENER_2(stopImpl
            , zfweakT<zfself>, owner
            , ZFListener, onLoadStop
            ) {
        if(onLoadStop) {
            owner->d->onLoadStopList.removeElement(onLoadStop);
        }
    } ZFLISTENER_END()
    taskId->stopImpl(stopImpl);
    if(onLoadStop) {
        d->onLoadStopList.add(onLoadStop);
    }
    if(d->loadStartTime != zftimetInvalid()) {
        return taskId;
    }
    d->loadStartTime = ZFTime::currentTime();
    zfobjRetain(this);
    d->index = 0;

    zfclassNotPOD _Impl {
    public:
        static void tryNext(ZF_IN ZFAdForRewardHelper *owner) {
            if(owner->d->index >= owner->d->cfgList.count()) {
                _stop(owner, zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("no valid impl"));
                return;
            }
            if(owner->timeout() != zftimetInvalid()
                    && ZFTime::currentTime() - owner->d->loadStartTime >= owner->timeout()
                    ) {
                _stop(owner, zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("load timeout"));
                return;
            }

            _ZFP_ZFAdForRewardHelperPrivate::Cfg const &cfg = owner->d->cfgList[(owner->d->index)++];
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

            zfobj<ZFAdForReward> impl;
            owner->d->impl = impl;
            zfweakT<ZFAdForRewardHelper> weakOwner = owner;
            ZFLISTENER_1(implOnLoadStop
                    , zfweakT<ZFAdForRewardHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                v_ZFResultType *resultType = zfargs.param0();
                switch(resultType->zfv()) {
                    case v_ZFResultType::e_Fail:
                        tryNext(weakOwner);
                        break;
                    case v_ZFResultType::e_Success:
                    default:
                        _stop(weakOwner, zfargs.param0(), zfargs.param1());
                        break;
                }
            } ZFLISTENER_END()
            impl->setup(cfg.implName, cfg.appId, cfg.adId);
            impl->load(implOnLoadStop);
        }
    private:
        static void _stop(
                ZF_IN ZFAdForRewardHelper *owner
                , ZF_IN v_ZFResultType *resultType
                , ZF_IN v_zfstring *errorHint
                ) {
            owner->d->index = 0;
            owner->d->loadStartTime = zftimetInvalid();
            owner->observerNotify(ZFAdForReward::E_AdOnLoadStop(), resultType, errorHint);

            ZFCoreArray<ZFListener> onLoadStopList;
            onLoadStopList.swap(owner->d->onLoadStopList);
            ZFArgs zfargs;
            zfargs
                .sender(owner)
                .param0(resultType)
                .param1(errorHint)
                ;
            for(zfindex i = 0; i < onLoadStopList.count(); ++i) {
                onLoadStopList[i].execute(zfargs);
            }
            zfobjRelease(owner);
        }
    };
    _Impl::tryNext(this);

    return taskId->stopImpl() ? taskId : zfnull;
}
ZFMETHOD_DEFINE_0(ZFAdForRewardHelper, zfbool, loaded) {
    return d->impl && d->impl->loaded();
}

ZFMETHOD_DEFINE_1(ZFAdForRewardHelper, void, start
        , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
        ) {
    if(d->started) {
        return;
    }
    d->started = zftrue;
    this->observerNotify(ZFAdForReward::E_AdOnStart(), this->window());

    zfself *owner = this;
    ZFLISTENER_2(onLoadStop
            , zfweakT<zfself>, owner
            , ZFListener, onStop
            ) {
        v_ZFResultType *resultType = zfargs.param0();
        if(owner == zfnull || owner->d->impl == zfnull
                || resultType == zfnull
                || resultType->zfv() != v_ZFResultType::e_Success
                ) {
            if(resultType == zfnull) {
                zfargs.param0(zfobj<v_ZFResultType>(v_ZFResultType::e_Fail));
            }
            onStop.execute(zfargs);
            return;
        }

        ZFLISTENER_1(AdOnDisplay
                , zfweakT<ZFAdForRewardHelper>, owner
                ) {
            if(!owner) {return;}
            owner->d->showing = zftrue;
            owner->observerNotify(ZFAdForReward::E_AdOnDisplay(), zfargs.param0(), zfargs.param1());
        } ZFLISTENER_END()

        ZFLISTENER_1(AdOnClick
                , zfweakT<ZFAdForRewardHelper>, owner
                ) {
            if(!owner) {return;}
            owner->observerNotify(ZFAdForReward::E_AdOnClick(), zfargs.param0(), zfargs.param1());
        } ZFLISTENER_END()

        ZFLISTENER_1(AdOnStop
                , zfweakT<ZFAdForRewardHelper>, owner
                ) {
            if(!owner) {return;}
            owner->d->started = zffalse;
            owner->d->showing = zffalse;
            ZFObserverGroupRemove(owner->d->observerOwner);
            owner->observerNotify(ZFAdForReward::E_AdOnStop(), zfargs.param0(), zfargs.param1());
        } ZFLISTENER_END()

        ZFObserverGroup(owner->d->observerOwner, owner->d->impl)
            .observerAdd(ZFAdForReward::E_AdOnDisplay(), AdOnDisplay)
            .observerAdd(ZFAdForReward::E_AdOnClick(), AdOnClick)
            .observerAdd(ZFAdForReward::E_AdOnStop(), AdOnStop)
            ;
        owner->d->impl->start();
    } ZFLISTENER_END()
    this->load(onLoadStop);
}
ZFMETHOD_DEFINE_0(ZFAdForRewardHelper, zfbool, started) {
    return d->started;
}

void ZFAdForRewardHelper::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForRewardHelperPrivate);
}
void ZFAdForRewardHelper::objectOnDealloc(void) {
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}

ZF_NAMESPACE_GLOBAL_END

