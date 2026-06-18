import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { devError } from '../../services/logger.js';

const cache = new Map();
const loader = new GLTFLoader();

// SkeletonUtils.clone() returns deep-cloned nodes that *share* geometries
// and materials with the source. Tagging cache-owned resources lets
// clearScene() skip disposing them — otherwise the next clone renders
// with disposed materials (default white).
function markShared(scene) {
  scene.traverse((obj) => {
    if (!obj.isMesh) return;
    if (obj.geometry) obj.geometry.userData.shared = true;
    const mats = Array.isArray(obj.material) ? obj.material : [obj.material];
    for (const mat of mats) {
      if (!mat) continue;
      mat.userData.shared = true;
      for (const key in mat) {
        const value = mat[key];
        if (value && value.isTexture) value.userData.shared = true;
      }
    }
  });
}

export function getModel(path) {
  if (cache.has(path)) {
    return cache.get(path);
  }

  const loadPromise = new Promise((resolve, reject) => {
    loader.load(path, (gltf) => {
      markShared(gltf.scene);
      resolve({
        model: gltf.scene,
        animations: gltf.animations
      });
    },
      undefined, // We can plug a LoadingManager in here later! To see a progress bar for example
      (error) => {
        devError(`Failed to load model at ${path}`, error);
        cache.delete(path);
        reject(error);
      }
    );
  });

  cache.set(path, loadPromise);
  return loadPromise;
}

// Full teardown path: dispose the GPU resources owned by every cached model
// and drop the cache so the next session reloads from disk.
export async function clearModelCache() {
  const entries = Array.from(cache.values());
  cache.clear();
  for (const promise of entries) {
    try {
      const { model } = await promise;
      model.traverse((obj) => {
        if (!obj.isMesh) return;
        if (obj.geometry) obj.geometry.dispose();
        const mats = Array.isArray(obj.material) ? obj.material : [obj.material];
        for (const mat of mats) {
          if (!mat) continue;
          for (const key in mat) {
            const value = mat[key];
            if (value && value.isTexture) value.dispose();
          }
          mat.dispose();
        }
      });
    } catch {
      // model never resolved; nothing to dispose
    }
  }
}

// TODO: check this out!
// Optional helper: Preload assets during a loading screen so gameplay is instantly smooth
export async function preloadModels(pathsArray) {
  await Promise.all(pathsArray.map(path => getModel(path)));
}
