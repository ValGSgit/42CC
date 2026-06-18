import * as THREE from 'three'


export const Cylinder = (radius, height, segments, materialConfig) => {
  const geometry = new THREE.CylinderGeometry(radius, radius, height, segments)
  let material;

  if (Array.isArray(materialConfig)) {
    material = materialConfig
  }
  else if (typeof materialConfig === 'string' || typeof materialConfig === 'number') {
    // B. User passed a Hex Color String (Legacy support)
    material = new THREE.MeshStandardMaterial({
      color: materialConfig,
      roughness: 0.8
    })
  }
  else {
    // C. User passed a single Material Object
    material = materialConfig
  }

  // 3. Create Mesh
  const mesh = new THREE.Mesh(geometry, material)
  mesh.receiveShadow = true

  return mesh
}

export const Box = (w, h, d, color) => {
  const geometry = new THREE.BoxGeometry(w, h, d)
  const material = new THREE.MeshStandardMaterial({ color, roughness: 0.8 })
  const mesh = new THREE.Mesh(geometry, material)
  mesh.castShadow = true
  mesh.receiveShadow = true
  return mesh
}
