# These examples use ESP_CORE_AVR (default), which requires a recompilation
# if ESP_CORE_ESP8266 was used previously, so perform a clean to be sure.
AVR_EXAMPLES := AutoBenchmark HelloDS3231Clock HelloStm32F1Clock \
	HelloStmRtcClock HelloSystemClockCoroutine HelloSystemClockLoop \
	MemoryBenchmark

# These examples use ESP_CORE_ESP8266, which requires a recompilation,
# if ESP_CORE_AVR was used previously, so perform a clean to be sure.
ESP8266_EXAMPLES := HelloEspSntpClock HelloNtpClock HelloNtpClockLazy

# The two sets of example apps must be compiled sequentially to prevent one set
# from interfering with the other. Various object files need to be compiled
# with different preprocessor macros. This calls 'make clean' at the very end
# so that targets that come after the EPOXY_CORE_ESP8266 examples to revert
# back to EPOXY_CORE__AVR.
all:
	$(MAKE) clean
	set -e; \
	for i in $(AVR_EXAMPLES); do \
		echo '==== Making:' $$i; \
		$(MAKE) -C $$i; \
	done; \
	$(MAKE) clean; \
	set -e; \
	for i in $(ESP8266_EXAMPLES); do \
		echo '==== Making:' $$i; \
		$(MAKE) -C $$i; \
	done; \
	$(MAKE) clean

clean:
	set -e; \
	for i in */Makefile; do \
		echo '==== Cleaning:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) clean; \
	done
