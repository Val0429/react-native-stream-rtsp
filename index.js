
import React, { Component } from 'react';
import {
  Platform,
  StyleSheet,
  Text,
  Button,
  Switch,
  Image,
  Dimensions,
  View,
  Alert,
  NativeModules,
  requireNativeComponent,
  findNodeHandle,
  AppRegistry
} from 'react-native';

const UIManager = NativeModules.UIManager;
const DispatchManager = UIManager.dispatchViewManagerCommand;
const Commands = UIManager.VideoView.Commands;

var NativeVideoView = requireNativeComponent('VideoView', VideoView, {
});
  
export class VideoView extends React.Component {
  constructor(props) {
    super(props);
  }
  
  Start(objs) {
	  DispatchManager(findNodeHandle(this.refs.native), Commands.what, ['Start', objs])
  }
  
  Stop() {
	  DispatchManager(findNodeHandle(this.refs.native), Commands.what, ['Stop'])
  }

  shouldComponentUpdate() {
    return false;
  }

  render(){
        return <NativeVideoView 
            ref="native"
            {...this.props}
            />;
    }
}

AppRegistry.registerComponent('VideoView', () => VideoView);