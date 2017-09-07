######################################
#
# inverted-switchbox
#
######################################

# where to find the source code - locally in this case
MONOSWITCHBOX_SITE_METHOD = local
MONOSWITCHBOX_SITE = $($(PKG)_PKGDIR)/

# even though this is a local build, we still need a version number
# bump this number if you need to force a rebuild
MONOSWITCHBOX_VERSION = 1

# dependencies (list of other buildroot packages, separated by space)
MONOSWITCHBOX_DEPENDENCIES =

# LV2 bundles that this package generates (space separated list)
MONOSWITCHBOX_BUNDLES = monoswitchBOX.lv2

# call make with the current arguments and path. "$(@D)" is the build directory.
MONOSWITCHBOX_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/source


# build command
define MONOSWITCHBOX_BUILD_CMDS
	$(MONOSWITCHBOX_TARGET_MAKE)
endef

# install command
define MONOSWITCHBOX_INSTALL_TARGET_CMDS
	$(MONOSWITCHBOX_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
endef


# import everything else from the buildroot generic package
$(eval $(generic-package))
