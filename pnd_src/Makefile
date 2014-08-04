
include ../scp.mk

.PHONY: c64_tools_tests
c64_tools_tests:
	./pnd_make.sh --genpxml ./genpxml.sh -d c64_tools_tests -x guess -p c64_tools_tests.pnd_DAT && cat c64_tools_tests.pnd_DAT c64_tools_icon-256x256.png >c64_tools_tests.pnd && scp c64_tools_tests.pnd  $(SCP_USER)@$(SCP_HOST):/sd1/pandora/apps/


.PHONY: validate
validate:
	 xmllint --noout --schema $(CROSS_ROOT)/usr/src/pandora-libraries/docs/PXML_schema.xsd c64_tools_tests/PXML.xml
