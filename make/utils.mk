comma:=,
empty:=
space:=$(empty) $(empty)
define newline


endef

define firstwords
$(wordlist 2, $(words $(1)),x $(1))
endef

define delimlist
$(foreach i,$(call firstwords,$(call firstwords,$(1))),$(i)$(2)) $(lastword $(call firstwords,$(1))) $(3) $(lastword $(1))
endef
