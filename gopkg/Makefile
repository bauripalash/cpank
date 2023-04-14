native:
	CGO_ENABLED=1 go build -o cpankgo  .
andapi:
	mkdir -p dist
	@echo "Building Android API"
	CGO_ENABLED=1 ANDROID_NDK_HOME=$(HOME)/Android/Sdk/ndk/25.1.8937393/ gomobile bind -v -o dist/panktijapi-$PANTKI_VERSION.aar -target=android -javapkg=in.palashbauri.panktijapi -androidapi 19 ./androidapi/
