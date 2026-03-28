#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
#
# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#
# CONFIG_JSON is the filename of the NPDM config file (.json), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.json
#     - config.json
#   If a JSON file is provided or autodetected, an ExeFS PFS0 (.nsp) is built instead
#   of a homebrew executable (.nro). This is intended to be used for sysmodules.
#   NACP building is skipped as well.
#---------------------------------------------------------------------------------
APP_TITLE	:=	JellyCar
APP_AUTHOR	:=	drakon
APP_VERSION	:=	1.1.0

TARGET		:=	JellyCar
BUILD		:=	build
SOURCES		:=	
DATA		:=	data
ASSETS_DIR	:=	../../Assets
ANDROMEDA_LIB	?=	../../../Andromeda-Lib
INCLUDES	:=	$(ANDROMEDA_LIB) $(ANDROMEDA_LIB)/Libs $(ANDROMEDA_LIB)/Libs/freetype_gl $(ANDROMEDA_LIB)/Libs/stb $(ANDROMEDA_LIB)/Libs/tinyxml $(ANDROMEDA_LIB)/Libs/tiny_obj_loader $(ANDROMEDA_LIB)/Libs/bullet/include/bullet \
				../../../JellyCar ../../../JellyCar/Libs
ROMFS	:=	romfs
FILE_LOG	?=	0
DEFINES		+=	-DANDROMEDA_FILE_LOG=$(FILE_LOG)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections \
			$(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ `aarch64-none-elf-pkg-config --cflags SDL2_mixer freetype2`

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11 -ffat-lto-objects -flto  -DANDROMEDA_SWITCH -D__SWITCH__ `aarch64-none-elf-pkg-config --cflags SDL2_mixer freetype2`
CFLAGS	+=	$(INCLUDE) -D__SWITCH__

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lnx -lglad -lEGL -lglapi -ldrm_nouveau -lpthread -lz -lm -lstdc++ -lfreetype -lpng -lbz2 `aarch64-none-elf-pkg-config --libs  sdl2 SDL2_mixer freetype2` 


#---------------------------------------------------------------------------------
# source files
#---------------------------------------------------------------------------------

SRC_RENDERER	:=\
	$(ANDROMEDA_LIB)/Andromeda/FileSystem/FileManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/FileSystem/BaseFile.cpp \
	$(ANDROMEDA_LIB)/Andromeda/FileSystem/Stdio/StdioFile.cpp \
	$(ANDROMEDA_LIB)/Andromeda/FileSystem/Stdio/StdioDirectory.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Utils/Logger.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Utils/SimpleProfiler.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Audio/Sound.o \
	$(ANDROMEDA_LIB)/Andromeda/Audio/AudioManager.o \
	$(ANDROMEDA_LIB)/Andromeda/Audio/Sdl/SdlSound.o \
	$(ANDROMEDA_LIB)/Andromeda/Audio/Sdl/SdlAudioManager.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Camera3d.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Texture.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/RenderManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Shader.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/ShaderManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Sprite.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/TextureManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/VertexArrayObject.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/VertexBufferObject.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/FrameBufferObject.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/RenderManagerGL3.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/ShaderGlsl.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/VertexArrayObjectGL3.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/VertexBufferObjectGL3.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/GL/FrameBufferObjectGL3.o \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Text/TextureAtlas.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Text/TexturedFont.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Text/FontManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Models/ModelObj.cpp \
	$(ANDROMEDA_LIB)/Andromeda/Graphics/Models/ModelMaterial.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/GameLoader.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/GameManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/GameState.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/SystemManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/MemoryManager.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/Timer.cpp \
	$(ANDROMEDA_LIB)/Andromeda/System/SWITCH/GameLoaderSwitch.o \
	$(ANDROMEDA_LIB)/Andromeda/System/SWITCH/MemoryManagerSwitch.o \
	$(ANDROMEDA_LIB)/Andromeda/Input/InputManager.o \
	$(ANDROMEDA_LIB)/Andromeda/Input/SWITCH/InputManagerSwitch.o \
	$(ANDROMEDA_LIB)/Andromeda/Input/SWITCH/GamepadDeviceSwitch.o \
	$(ANDROMEDA_LIB)/Andromeda/Input/SWITCH/TouchDeviceSwitch.o \
	$(ANDROMEDA_LIB)/Libs/tinyxml/tinystr.cpp \
	$(ANDROMEDA_LIB)/Libs/tinyxml/tinyxmlerror.cpp \
	$(ANDROMEDA_LIB)/Libs/tinyxml/tinyxmlparser.cpp \
	$(ANDROMEDA_LIB)/Libs/tinyxml/tinyxml.cpp \
	$(ANDROMEDA_LIB)/Libs/tiny_obj_loader/tiny_obj_loader.cpp\
	../../../JellyCar/JellyPhysics/AABB.cpp \
	../../../JellyCar/JellyPhysics/Body.cpp \
	../../../JellyCar/JellyPhysics/ClosedShape.cpp \
	../../../JellyCar/JellyPhysics/InternalSpring.cpp \
	../../../JellyCar/JellyPhysics/PointMass.cpp \
	../../../JellyCar/JellyPhysics/PressureBody.cpp \
	../../../JellyCar/JellyPhysics/SpringBody.cpp \
	../../../JellyCar/JellyPhysics/Vector2.cpp \
	../../../JellyCar/JellyPhysics/VectorTools.cpp \
	../../../JellyCar/JellyPhysics/World.cpp \
	../../../JellyCar/JellyCar/Car/Car.cpp \
	../../../JellyCar/JellyCar/Car/Chassis.cpp \
	../../../JellyCar/JellyCar/Car/GameBallonBody.cpp \
	../../../JellyCar/JellyCar/Car/SuspensionShock.cpp \
	../../../JellyCar/JellyCar/Car/Tire.cpp \
	../../../JellyCar/JellyCar/Levels/KinematicControl.cpp \
	../../../JellyCar/JellyCar/Levels/KinematicMotor.cpp \
	../../../JellyCar/JellyCar/Levels/KinematicPlatform.cpp \
	../../../JellyCar/JellyCar/Levels/LevelManager.cpp \
	../../../JellyCar/JellyCar/Levels/LevelSoftBody.cpp \
	../../../JellyCar/JellyCar/Levels/GamePressureBody.cpp \
	../../../JellyCar/JellyCar/Levels/GameSpringBody.cpp \
	../../../JellyCar/JellyCar/Utils/JellyHelper.cpp \
	../../../JellyCar/JellyCar/Utils/InputHelper.cpp \
	../../../JellyCar/JellyCar/Utils/AudioHelper.cpp \
	../../../JellyCar/JellyCar/Mods/ModLoader.cpp \
	../../../JellyCar/JellyCar/Game/JellyCore.cpp \
	../../../JellyCar/JellyCar/Game/JellyGame.cpp \
	../../../JellyCar/JellyCar/Game/JellyOptions.cpp \
	../../../JellyCar/JellyCar/Game/JellyMenuBetter.cpp \
	../../../JellyCar/JellyCar/Game/JellyIntro.cpp \
	../../../JellyCar/JellyCar/Game/JellySplash.cpp \
	../../../JellyCar/JellyCar/JellyGameManager.cpp \
	../../../JellyCar/JellyCar/JellyCar.cpp
	
	
	
SOURCEFILES_CPP	:=\
	$(SRC_RENDERER) 

SOURCEFILES_C	:=\
	$(ANDROMEDA_LIB)/Libs/freetype_gl/distance-field.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/edtaa3func.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/font-manager.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/platform.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/texture-atlas.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/texture-font.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/utf8-utils.c \
	$(ANDROMEDA_LIB)/Libs/freetype_gl/vector.c

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

resolve_path = $(if $(filter /%,$(1)),$(1),$(CURDIR)/$(1))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)
export ROMFS_SRC	:=	$(call resolve_path,$(ASSETS_DIR))
export ROMFS_DST	:=	$(CURDIR)/$(ROMFS)/Assets
export ROMFS_STAMP	:=	$(CURDIR)/$(ROMFS)/.assets-staged

export VPATH	:=	$(foreach dir,$(SOURCES),$(call resolve_path,$(dir))) \
					$(foreach sf,$(SOURCEFILES_CPP),$(call resolve_path,$(dir $(sf)))) \
					$(foreach sf,$(SOURCEFILES_C),$(call resolve_path,$(dir $(sf)))) \
					$(foreach dir,$(DATA),$(call resolve_path,$(dir)))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c))) $(foreach f,$(SOURCEFILES_C),$(notdir $(f)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp))) $(foreach f,$(SOURCEFILES_CPP),$(notdir $(f)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(call resolve_path,$(dir))) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(call resolve_path,$(ROMFS))
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(ROMFS_STAMP):
	@[ -d $(ROMFS_SRC) ] || (echo "Missing assets directory: $(ROMFS_SRC)" && false)
	@[ -d $(CURDIR)/$(ROMFS) ] || mkdir -p $(CURDIR)/$(ROMFS)
	@rm -rf $(ROMFS_DST)
	@cp -a $(ROMFS_SRC) $(ROMFS_DST)
	@if [ -d "$(ROMFS_DST)/Shaders/GL" ]; then \
		for shader in "$(ROMFS_DST)"/Shaders/GL/*.vert "$(ROMFS_DST)"/Shaders/GL/*.frag; do \
			[ -e "$$shader" ] || continue; \
			cp -f "$$shader" "$(ROMFS_DST)/Shaders/$$(basename "$$shader")"; \
		done; \
	fi
	@touch $(ROMFS_STAMP)

$(BUILD): $(ROMFS_STAMP)
	@grep -Fq "Switch cleanup: game manager done" $(call resolve_path,$(ANDROMEDA_LIB))/Andromeda/System/SWITCH/GameLoaderSwitch.cpp || (echo "Andromeda-Lib is missing Switch cleanup patch. Apply Build/Switch/andromeda-lib-libnx.patch."; false)
	@grep -Fq "Switch return to libnx exit" $(call resolve_path,$(ANDROMEDA_LIB))/Andromeda/System/SWITCH/GameLoaderSwitch.cpp || (echo "Andromeda-Lib is missing Switch libnx-exit patch. Apply Build/Switch/andromeda-lib-libnx.patch."; false)
	@grep -Fq "TexturedFont::LoadFromMemory: texture_font_new_from_memory failed" $(call resolve_path,$(ANDROMEDA_LIB))/Andromeda/Graphics/Text/TexturedFont.cpp || (echo "Andromeda-Lib is missing TexturedFont patch. Apply Build/Switch/andromeda-lib-libnx.patch."; false)
	@grep -Fq "FT_Set_Pixel_Sizes(*face, 0, (FT_UInt)logical_size);" $(call resolve_path,$(ANDROMEDA_LIB))/Libs/freetype_gl/texture-font.c || (echo "Andromeda-Lib is missing freetype_gl font patch. Apply Build/Switch/andromeda-lib-libnx.patch."; false)
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
ifeq ($(strip $(APP_JSON)),)
	@rm -fr $(BUILD) $(TARGET).nro $(TARGET).nacp $(TARGET).elf $(ROMFS)
else
	@rm -fr $(BUILD) $(TARGET).nsp $(TARGET).nso $(TARGET).npdm $(TARGET).elf $(ROMFS)
endif


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(APP_JSON)),)

all	:	$(OUTPUT).nro

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

else

all	:	$(OUTPUT).nsp

$(OUTPUT).nsp	:	$(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).nso	:	$(OUTPUT).elf

endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
