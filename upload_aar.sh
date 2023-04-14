#!/bin/env sh
export PANKTI_VERSION=$(cat ./VERSION)
cd dist
echo "v${PANKTI_VERSION}"
echo "Upload .AAR"
mvn gpg:sign-and-deploy-file -Durl=https://s01.oss.sonatype.org/service/local/staging/deploy/maven2/ -DrepositoryId=ossrh -DpomFile=panktijapi-$PANKTI_VERSION.pom -Dfile=panktijapi-$PANTKI_VERSION.aar
echo "Upload sources.JAR"
mvn gpg:sign-and-deploy-file -Durl=https://s01.oss.sonatype.org/service/local/staging/deploy/maven2/ -DrepositoryId=ossrh -DpomFile=panktijapi-$PANKTI_VERSION.pom -Dfile=panktijapi-$PANTKI_VERSION-sources.jar -Dclassifier=sources
