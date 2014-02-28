TARGET = testMuxer

LIBS += -lavcodec -lavformat -lavutil -lswresample -lswscale -lm

SOURCES += \
	   VideoScale.cpp      \
	   AudioResample.cpp   \
	   MediaSource.cpp     \
	   testMuxer.cpp       \
	   VideoEncode.cpp     \
	   MediaEncode.cpp     \
	   AudioEncode.cpp     \
	   MediaMuxer.cpp      \

OBJS += \
	AudioResample.o   \
	VideoScale.o      \
	MediaSource.o     \
	MediaMuxer.o      \
	MediaEncode.o     \
	AudioEncode.o     \
	VideoEncode.o     \
	testMuxer.o       \

DEPENDS += \
	   VideoScale.d       \
	   AudioResample.d    \
	   testMediaSource.d  \
	   MediaSource.d      \
	   MediaEncode.d      \
	   AudioEncode.d      \
	   VideoEncode.d      \
	   testMuxer.d        \

%.o:%.cpp
	g++ -g -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

all: $(TARGET)

$(TARGET) : $(OBJS) $(SOURCES)
	g++ -o $(TARGET) $(OBJS) $(LIBS)

clean:
	$(RM) $(DEPENDS) $(OBJS) $(TARGET)
