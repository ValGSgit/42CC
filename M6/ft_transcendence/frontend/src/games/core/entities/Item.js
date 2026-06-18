import * as THREE from 'three';

export const shopItems = [
  { name: 'Barn', path: '/models/barn.glb', icon: '/icons/Barn.png', cost: 100, type: 'item' },
  { name: 'Barrel', path: '/models/barrel.glb', icon: '/icons/Barrel.png', cost: 20, type: 'item' },
  { name: 'Fence', path: '/models/fence.glb', icon: '/icons/Fence.png', cost: 15, type: 'item' },
  { name: 'Fence Gate', path: '/models/fenceGate.glb', icon: '/icons/Fence Gate.png', cost: 20, type: 'item' },
  { name: 'Fence Pole', path: '/models/fencePole.glb', icon: '/icons/Fence Pole.png', cost: 10, type: 'item' },
  { name: 'Grass', path: '/models/grass.glb', icon: '/icons/Grass.png', cost: 2, type: 'decoration' },
  { name: 'Hay', path: '/models/hay.glb', icon: '/icons/Hay.png', cost: 25, type: 'item' },
  { name: 'Manger', path: '/models/manger.glb', icon: '/icons/Manger.png', cost: 50, type: 'item' },
  { name: 'Stones', path: '/models/stones.glb', icon: '/icons/Stones.png', cost: 5, type: 'decoration' },
  { name: 'Tree', path: '/models/tree.glb', icon: '/icons/Tree.png', cost: 10, type: 'item' },
  { name: 'Water Trough', path: '/models/waterTrough.glb', icon: '/icons/Water Trough.png', cost: 50, type: 'item' },
  { name: 'Wheat', path: '/models/wheat.glb', icon: '/icons/Wheat.png', cost: 50, type: 'decoration' },
]
export class Item {
  constructor(model, animations, options = {}) {
    this.model = model;
    this.animations = animations;

    const pos = options.position || [0, 0, 0];
    const rotation = options.rotation || 0;
    const scale = options.scale || [1, 1, 1];

    this.model.position.set(...pos);
    this.model.rotation.set(0, rotation, 0);
    this.model.scale.set(...scale);
    this.model.updateMatrixWorld(true);

    if (this.animations && this.animations.length > 1) {
      this.mixer = new THREE.AnimationMixer(this.model);
      const action = this.mixer.clipAction(this.animations[0]);
      action.play();
    }
  }

  update(delta) {
    if (this.mixer) {
      this.mixer.update(delta);
    }
  }
}