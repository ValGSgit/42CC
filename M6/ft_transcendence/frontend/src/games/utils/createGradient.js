import * as THREE from 'three'

export function Linear(colorTop, colorBottom) {
  const canvas = document.createElement('canvas')
  canvas.width = 32
  canvas.height = 32

  const context = canvas.getContext('2d')

  const gradient = context.createLinearGradient(0, 0, 0, 32)

  gradient.addColorStop(0, colorTop)
  gradient.addColorStop(1, colorBottom)

  context.fillStyle = gradient
  context.fillRect(0, 0, 32, 32)

  const texture = new THREE.CanvasTexture(canvas)
  texture.colorSpace = THREE.SRGBColorSpace

  return texture
}

export function Radial(colorInner, colorOutter) {
  const size = 128
  const center = size / 2

  const canvas = document.createElement('canvas');
  canvas.width = size;
  canvas.height = size;

  const context = canvas.getContext('2d');

  const gradient = context.createRadialGradient(center, center, 0, center, center, center);

  gradient.addColorStop(0, colorInner);
  gradient.addColorStop(1, colorOutter);

  context.fillStyle = gradient;
  context.fillRect(0, 0, size, size);

  const texture = new THREE.CanvasTexture(canvas);
  texture.colorSpace = THREE.SRGBColorSpace

  return texture;
}