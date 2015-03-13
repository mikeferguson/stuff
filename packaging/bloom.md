# 3rd party release

http://wiki.ros.org/bloom/Tutorials/ReleaseThirdParty

 * add package.xml
 * git-bloom-config copy indigo jade
 * git-bloom-config edit jade (be sure to change rev to -1)
 * git-bloom-release jade (so there is a release to patch)
 * git checkout patches/release/jade/sbpl
 * edit CMakeLists.txt to install package.xml
 * commit the change
 * git-bloom-patch export
 * git push --all
 * git-bloom-release jade (again, this is the real release)
 * bloom-release -r jade -t jade sbpl --pull-request-only
