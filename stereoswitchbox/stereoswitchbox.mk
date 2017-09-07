######################################
#
# stereoswitchbox
#
######################################

# where to find the source code - locally in this case
STEREOSWITCHBOX_SITE_METHOD = local
STEREOSWITCHBOX_SITE = $($(PKG)_PKGDIR)/

# even though this is a local build, we still need a version number
# bump this number if you need to force a rebuild
STEREOSWITCHBOX_VERSION = 1

# dependencies (list of other buildroot packages, separated by space)
STEREOSWITCHBOX_DEPENDENCIES =

# LV2 bundles that this package generates (space separated list)
STEREOSWITCHBOX_BUNDLES = stereoswitchBOX.lv2

# call make with the current arguments and path. "$(@D)" is the build directory.
STEREOSWITCH_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/source


# build command
define STEREOSWITCHBOX_BUILD_CMDS
	$(STEREOSWITCHBOX_TARGET_MAKE)
endef

# install command
define STEREOSWITCHBOX_INSTALL_TARGET_CMDS
	$(STEREOSWITCHBOX_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
endef


# import everything else from the buildroot generic package
$(eval $(generic-package))
