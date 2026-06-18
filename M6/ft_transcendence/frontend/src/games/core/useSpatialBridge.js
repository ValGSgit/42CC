import { ref } from 'vue'
import { gEngine, gPlayer } from './globals.js'
import { debug, devWarn, devError } from '../../services/logger.js'
import { CONST } from '../config/constants.js'
import * as THREE from 'three'

let hudScene
let hudCamera

export function useSpatialBridge(onSpatialData) {
  const socket = ref(null)
  const isConnected = ref(false)
    if (!CONST.AR_ENABLED)
    return {isConnected}
  const initSpatialBridge = (url = 'ws://localhost:8082') => {
    socket.value = new WebSocket(url)

    socket.value.onopen = () => {
      isConnected.value = true
      debug('✅ Spatial Bridge Connected')
    }

    socket.value.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data)
        // Send the parsed JSON back to the callback function
        onSpatialData(data)
      } catch (err) {
        // Silently ignore non-JSON heartbeats
      }
    }

    socket.value.onclose = () => {
      isConnected.value = false
      devWarn('❌ Spatial Bridge disconnected. Reconnecting...')
      setTimeout(() => initSpatialBridge(url), 3000)
    }

    socket.value.onerror = (err) => {
      devError('Spatial Bridge Error:', err)
    }
  }

  const closeSpatialBridge = () => {
    if (socket.value) {
      socket.value.close()
    }
  }

  return {
    isConnected,
    initSpatialBridge,
    closeSpatialBridge
  }
}

export function init_redot(){
  // Setup a separate HUD scene
  hudScene = new THREE.Scene();
  hudCamera = new THREE.OrthographicCamera(-window.innerWidth / 2, window.innerWidth / 2, window.innerHeight / 2, -window.innerHeight / 2, 1, 10);
  hudCamera.position.z = 10;
  
  // Create the Red Dot Geometry
  const dotGeo = new THREE.CircleGeometry(6, 32);
  const dotMat = new THREE.MeshBasicMaterial({ 
    color: 0xff0000, 
    depthTest: false, // Always render on top
    transparent: true,
    opacity: 0.8
  });
  const handDot = new THREE.Mesh(dotGeo, dotMat);
  hudScene.add(handDot);
  
  // Store in global engine for access
  gEngine.value.handDot = handDot;
}

export function render_redot(){
    gEngine.value.renderer.autoClear = false;
    gEngine.value.renderer.clearDepth();
    gEngine.value.renderer.render(hudScene, hudCamera);
}

export function handInput(data){
  if (gEngine.value.handDot) {
    const dot = gEngine.value.handDot;
    const orthoX = data.x * 1.5 - window.innerWidth / 2;
    const orthoY = -(data.y - window.innerHeight / 2); // Flip Y
    dot.position.set(orthoX, orthoY, 1);
    const scale = data.pinch ? 0.7 : 1.0;
    dot.scale.set(scale, scale, 1);
    dot.material.color.setHex(data.pinch ? 0xffff00 : 0xff0000); // Turn yellow on pinch
    if (data.pinch) {
      if (gPlayer.value) gPlayer.value.spitToPoint(data.x * 1.5, data.y);
    }
  }
}

export function headInput(data){
  const scale = 1;
  gEngine.value.spatialOffset = {
    x: -data.x * scale, // Keep your axis flips here
    y: data.y * scale,
    z: data.z * scale
  };
  gEngine.value.spatialRotation = new THREE.Quaternion(data.qx, data.qy, data.qz, data.qw);
}