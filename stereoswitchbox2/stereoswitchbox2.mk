######################################
#
# stereoswitchbox2
#
######################################

# where to find the source code - locally in this case
STEREOSWITCHBOX2_SITE_METHOD = local
STEREOSWITCHBOX2_SITE = $($(PKG)_PKGDIR)/

# even though this is a local build, we still need a version number
# bump this number if you need to force a rebuild
STEREOSWITCHBOX2_VERSION = 1

# dependencies (list of other buildroot packages, separated by space)
STEREOSWITCHBOX2_DEPENDENCIES =

# LV2 bundles that this package generates (space separated list)
STEREOSWITCHBOX2_BUNDLES = stereoswitchbox2.lv2

# call make with the current arguments and path. "$(@D)" is the build directory.
STEREOSWITCHBOX2_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/source


# build command
define STEREOSWITCHBOX2_BUILD_CMDS
	$(STEREOSWITCHBOX2_TARGET_MAKE)
endef

# install command
define STEREOSWITCHBOX2_INSTALL_TARGET_CMDS
	$(STEREOSWITCHBOX2_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
endef


# import everything else from the buildroot generic package
$(eval $(generic-package))
