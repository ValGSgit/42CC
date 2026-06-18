import * as THREE from 'three'

// These materials are module-level singletons re-used across the scene
// (collider boxes, ghosts, projectiles, etc.). clearScene() must not dispose
// them or the next session renders with broken/white meshes.
function shared(material) {
  material.userData.shared = true
  return material
}

export const MATERIALS = {
  debug: shared(new THREE.MeshBasicMaterial({
    color: 0x00ff00,
    wireframe: true,
    visible: true,
  })),

  collider: shared(new THREE.MeshBasicMaterial({
    visible: false
  })),

  collider_hit: shared(new THREE.MeshBasicMaterial({
    color: 0xff0000,
    wireframe: true,
  })),

  highlight: shared(new THREE.MeshStandardMaterial({
    color: 0x33ffff,
    emissive: 0x33ffff,
    emissiveIntensity: 0.7,
    transparent: true,
    opacity: 0.5,
  })),

  ghost: shared(new THREE.MeshBasicMaterial({
    color: 0xff0000,
    transparent: true,
    opacity: 0.5,
  })),

  spit: shared(new THREE.MeshStandardMaterial({
    color: 0x00ffff,
    emissive: 0x00ffff,
    emissiveIntensity: 2,
    transparent: true,
    opacity: 0.8
  })),
}

export const ROAD_MATERIALS = {
  obstacle: shared(new THREE.MeshStandardMaterial({
    color: '#550000',
    roughness: 0.8
  }))
}
