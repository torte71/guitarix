import inspect, pprint, os
from waflib import Configure
from waflib.Errors import WafError
from waflib.TaskGen import feature, before_method
from waflib.Context import g_module

# What was intention of lv2_prepare_link()?
# env.cxxshlib_PATTERN is already correctly set by waf ('%s.so' on Linux and '%s.dll' on Windows)
@feature('lv2')
@before_method('apply_link')
def lv2_prepare_link(self):
    return
    #self.env.cxxshlib_PATTERN = '%s.so'

def get_lv2_base(bld, kw, nframes=3):
    try:
        lv2_base = kw['lv2_base']
    except KeyError:
        pass
    else:
        return lv2_base, '${LV2DIR}/%s.lv2' % lv2_base
    cf = inspect.currentframe()
    for i in range(nframes):
        cf = cf.f_back
    fi = inspect.getframeinfo(cf)
    fn = bld.root.make_node(fi.filename).srcpath()
    raise WafError(
        '{}[{}]: lv2_base not set in lv2 task generator:\n{}'.format(
            fn, fi.lineno, pprint.pformat(kw)))

def lv2_add_common(tg, target, install_path, defines=None, linkflags=None, cxxflags=None):
    tg.features.append('lv2')
    if not tg.target:
        tg.target = target
    if not hasattr(tg, 'install_path'):
        tg.install_path = install_path
    if defines:
        tg.defines = tg.to_list(getattr(tg, 'defines', [])) + tg.to_list(defines)
    if linkflags:
        tg.linkflags = tg.to_list(getattr(tg, 'linkflags', [])) + tg.to_list(linkflags)
    if cxxflags:
        tg.cxxflags = tg.to_list(getattr(tg, 'cxxflags', [])) + tg.to_list(cxxflags)

@Configure.conf
def lv2(bld, *k, **kw):
    lv2_base, dst = get_lv2_base(bld, kw)
    tg = bld.shlib(features='strip', *k, **kw)
    cxxflags = []
    if not bld.env['OPT'] and bld.env['SSE2']:
        cxxflags = [ "-msse2", "-mfpmath=sse"]
    lv2_add_common(tg, lv2_base, dst, ["LV2_SO"], cxxflags=cxxflags + bld.env['OS_LV2_CXXFLAGS'])
    return tg

@Configure.conf
def lv2_gui(bld, *k, **kw):
    if not bld.env['LV2GUI']:
        return None
    lv2_base, dst = get_lv2_base(bld, kw)
    tg = bld.shlib(features='strip', *k, **kw)
    lv2_add_common(tg, lv2_base+'_gui', dst, ["LV2_GUI"], bld.env['OS_LV2_CXXFLAGS'])
    return tg

@Configure.conf
def lv2_ttl(bld, *k, **kw):
    lv2_base, dst = get_lv2_base(bld, kw)
    # OS specific names in *.ttl files:
    if bld.env.OS == 'win32':
        lv2ui_type = 'WindowsUI'
        lib_ext = '.dll'
    else:
        lv2ui_type = 'X11UI'
        lib_ext = '.so'
    # always set extension (.so / .dll)
    sub_list = [('@LIB_EXT@', lib_ext)]
    # select X11UI / WindowsUI
    # the whole (unprocessed) paragraph could be removed, if LV2GUI is unset
    # otoh LV2 simply ignores settings for unknown or unsupported "guiext:" (in this case "@LV2UI_TYPE@")
    if bld.env.LV2GUI:
        sub_list.extend( [('@LV2UI_TYPE@', lv2ui_type)] )
    # remove reference to modgui ttl files (if not used)
    if not bld.env.MODGUI:
        sub_list.extend( [(',\n        <modgui.ttl> ', ' '),
                         (', <modgui.ttl>', ' '),
                         (', <modguis.ttl>', ' ')] )
    # update manifest.ttl (no gui)
    tg = bld(rule     = g_module.sub_file,
            source   = 'manifest.ttl.in',
            target   = 'manifest.ttl',
            sub_list = sub_list,
            install_path = '${LV2DIR}/%s.lv2' % lv2_base,
            )
    # update $PLUGIN.ttl (gui)
    if bld.env['LV2GUI']:
        bld(rule     = g_module.sub_file,
            source   = '%s.ttl.in' % lv2_base,
            target   = '%s.ttl' % lv2_base,
            sub_list = sub_list,
            install_path = '${LV2DIR}/%s.lv2' % lv2_base,
            )
    # add ttl/modgui stuff to install list
    if bld.env['MODGUI']:
        bld.install_files(dst, bld.path.ant_glob('*.ttl'), relative_trick=True)
        bld.install_files(dst, bld.path.ant_glob('modgui/**/*'), relative_trick=True)
    else:
        bld.install_files(dst, bld.path.ant_glob('*.ttl', excl=['modgui.ttl']), relative_trick=True)
    # do I have to return something here?
    return tg

def options(opt):
    lv2 = opt.add_option_group("LV2 Options (installing LV2 modules)")

    lv2.add_option('--no-lv2',
                    dest='lv2',
                    action='store_false',
                    default=True,
                    help=("Don't build LV2 plugins (Default no)"))

    lv2.add_option('--no-lv2-gui',
                    dest='lv2_gui',
                    action='store_false',
                    default=True,
                    help=("Don't build LV2 plugin GUI's (Default no)"))

    lv2.add_option('--mod-lv2',
                    dest='mod_lv2',
                    action='store_true',
                    default=False,
                    help=("Install MOD LV2 GUI's (Default no)"))

    lv2.add_option('--lv2dir',
                    type='string',
                    help='LV2 plugin directory [Default: <prefix>/lib/lv2]')

    lv2.add_option('--disable-sse',
                    action='store_const',
                    default=False,
                    const=True,
                    help=("disable SSE for lv2 plugins (ARM)"))

def configure(conf):
    opt = conf.options
    if not opt.lv2:
        return
    env = conf.env
    env.LV2 = True
    env.LV2GUI = opt.lv2_gui
    env.MODGUI = opt.mod_lv2
    env.NOSSE =  opt.disable_sse
    conf.check_cfg(package='sigc++-2.0', args='--cflags --libs', uselib_store='SIGC', mandatory=1)
    conf.check_cfg(package='lv2', args=['--cflags','--libs','lv2 >= 1.2.0'], uselib_store='LV2CORE', mandatory=1)
    conf.define_with_env('GX_LV2_STYLE_DIR', os.path.join(env.GX_STYLE_DIR,'LV2'))
    if opt.lv2dir:
        env.LV2DIR = opt.lv2dir
    else:
        env.LV2DIR = os.path.normpath(os.path.join(env.LIBDIR, 'lv2'))
