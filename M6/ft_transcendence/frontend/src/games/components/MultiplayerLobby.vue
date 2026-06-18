<template>
  <div class="modal-overlay">
    <div v-if="gMinigame.mode === 2">
      <div v-if="!gMinigame.currentRoomName" class="shop-title">
        Spit Royale Lobby
        <button class="shop-btn" @click="handleCreateRoom(gMinigame.mode)" title="Create New Room">
          Create New Room
        </button>
        <button v-if="spitRooms.length > 0" class="shop-btn" @click="handleJoinRandom(gMinigame.mode)" title="Join Random Room">
          Join Random Room
        </button>
        <div v-for="room in spitRooms" :key="room.id">
          <button class="shop-btn" @click="handleJoinRoom(room.id)" title="Join Room">
            Join {{ room.name }} (Lv {{ room.averageLevel ?? 1 }}, {{ room.playerCount }}/10)
          </button>
        </div>
        <button class="close-btn" @click="closeLobbyMenu()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
      <div v-else class="shop-title">
        {{ gMinigame.currentRoomName }} ({{ gMinigame.players.length }}/10)
        <div v-for="player in gMinigame.players" :key="player.id" class="stat-row">
          {{ player.name }} - {{ player.isReady ? '✅ Ready' : '⏳ Waiting' }}
        </div>
        <div v-if="gMinigame.players.length < 2" class="stat-row">⏳ Waiting for another player to join…</div>
        <button class="shop-btn" @click="toggleReady">
          {{ gMinigame.isReady ? 'Cancel Ready' : 'Ready Up' }}
        </button>
        <button class="close-btn" @click="leaveRoom()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>
    <div v-if="gMinigame.mode === 4">
      <div v-if="!gMinigame.currentRoomName" class="shop-title">
        Alpaca Road Lobby
        <button class="shop-btn" @click="handleCreateRoom(gMinigame.mode)" title="Create New Room">
          Create New Room
        </button>
        <div v-for="room in roadRooms" :key="room.id">
          <button class="shop-btn" @click="handleJoinRoom(room.id)">
            Join {{ room.name }} (Lv {{ room.averageLevel ?? 1 }}, {{ room.playerCount }}/4)
          </button>
        </div>
        <button class="close-btn" @click="closeLobbyMenu()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
      <div v-else class="shop-title"> 
        {{ gMinigame.currentRoomName }} ({{ gMinigame.players.length }}/4)
        <div v-for="player in gMinigame.players" :key="player.id" class="stat-row">
          {{ player.name }} - {{ player.isReady ? '✅ Ready' : '⏳ Waiting' }}
        </div>
        <button class="shop-btn" @click="toggleReady">
          {{ gMinigame.isReady ? 'Cancel Ready' : 'Ready Up' }}
        </button>
        <button class="close-btn" @click="leaveRoom()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>
  </div>
</template>

<script setup>
import { computed, onMounted, onUnmounted } from 'vue';
import AppIcon from '../../components/AppIcon.vue';
import { useAuthStore } from '../../stores/auth.js';
import { gMinigame, gPlayer, gUser } from '../core/globals.js';
import { useUIManager } from '../core/useUIManager.js';
import { activeClient } from '../mini_games/GameClient.js';
import { changeGame } from '../mini_games/init.js';

const { closeLobbyMenu } = useUIManager()
const authStore = useAuthStore()

const spitRooms = computed(() => {
  if (!gMinigame.value.publicRooms) return [];
  return gMinigame.value.publicRooms.filter(room => room.gameType === 2);
});

const roadRooms = computed(() => {
  if (!gMinigame.value.publicRooms) return [];
  return gMinigame.value.publicRooms.filter(room => room.gameType === 4);
});

onMounted(() => {
  activeClient.connect();
});

onUnmounted(() => {
  if (!gMinigame.value.isActive && !gMinigame.value.isReady) {
    activeClient.disconnect();
    gMinigame.value.currentRoomName = null;
    gMinigame.value.isReady = false;
    changeGame();
  }
});

const handleCreateRoom = (gameType) => {
  const name = gUser.value.name || "Vue_Alpaca";
  const color = gPlayer.value?.color || "#ffffff";
  activeClient.createRoom(name, color, gameType);
};

const handleJoinRoom = (roomId) => {
  const name = gUser.value.name || "Vue_Alpaca";
  const color = gPlayer.value?.color || "#ffffff";
  activeClient.joinRoom(name, roomId, color);
};

const handleJoinRandom = (gameType) => {
  if (gameType !== 2) return;

  const rooms = spitRooms.value;
  if (rooms.length > 0) {
    const myLevel = Number(authStore.user?.level ?? 1)
    const closestRoom = [...rooms].sort((a, b) => {
      const aDelta = Math.abs((a.averageLevel ?? 1) - myLevel)
      const bDelta = Math.abs((b.averageLevel ?? 1) - myLevel)
      return aDelta - bDelta
    })[0]
    handleJoinRoom(closestRoom.id);
  }
};

const toggleReady = () => {
  gMinigame.value.isReady = !gMinigame.value.isReady;
  activeClient.sendReady(gMinigame.value.isReady);
};

const leaveRoom = () => {
  activeClient.leaveRoom();
  gMinigame.value.currentRoomName = null;
  gMinigame.value.isReady = false;
};

</script>

<!---------------------- STYLE ------------------------->
<style src="../../styles/games/game.css" scoped></style>