# name of your application
APPLICATION = gas-leaks-and-fires-notifier

# If no BOARD is found in the environment, use this default:
BOARD ?= b-l072z-lrwan1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules
USEMODULE += sx1276
USEMODULE += semtech_loramac_rx
USEMODULE += xtimer
USEMODULE += shell
USEMODULE += ps

USEPKG += semtech-loramac
LORA_REGION ?= EU868


include $(RIOTBASE)/Makefile.include