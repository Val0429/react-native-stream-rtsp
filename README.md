
# react-native-stream-rtsp

## Getting started

`$ npm install react-native-stream-rtsp --save`

### Mostly automatic installation

`$ react-native link react-native-stream-rtsp`

### Manual installation


#### iOS

1. In XCode, in the project navigator, right click `Libraries` ➜ `Add Files to [your project's name]`
2. Go to `node_modules` ➜ `react-native-stream-rtsp` and add `RNStreamRtsp.xcodeproj`
3. In XCode, in the project navigator, select your project. Add `libRNStreamRtsp.a` to your project's `Build Phases` ➜ `Link Binary With Libraries`
4. Run your project (`Cmd+R`)<

#### Android

1. Open up `android/app/src/main/java/[...]/MainActivity.java`
  - Add `import com.isap.RNStreamRtspPackage;` to the imports at the top of the file
  - Add `new RNStreamRtspPackage()` to the list returned by the `getPackages()` method
2. Append the following lines to `android/settings.gradle`:
  	```
  	include ':react-native-stream-rtsp'
  	project(':react-native-stream-rtsp').projectDir = new File(rootProject.projectDir, 	'../node_modules/react-native-stream-rtsp/android')
  	```
3. Insert the following lines inside the dependencies block in `android/app/build.gradle`:
  	```
      compile project(':react-native-stream-rtsp')
  	```


## Usage
```javascript
import RNStreamRtsp from 'react-native-stream-rtsp';

// TODO: What to do with the module?
RNStreamRtsp;
```
  