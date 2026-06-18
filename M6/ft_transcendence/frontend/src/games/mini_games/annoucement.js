let activeTimeouts = [];

export function makeAnnouncement(text, time) {
  const el = document.createElement('div');
  el.className = 'announcement';
  el.innerText = text

  document.body.appendChild(el);

  const timeout = setTimeout(() => {
    if (el.parentNode) {
      el.remove();
    }
  }, time);

  activeTimeouts.push(timeout);
}

export function playCountDown(seconds = 3) {
  makeAnnouncement('Get Ready!', 1000);
  for (let i = 0; i < seconds; i++) {
    const timeout = setTimeout(() => {
      makeAnnouncement(seconds - i, 1000);
    }, 1000 * i + 1000);
    activeTimeouts.push(timeout);
  }
  const timeout = setTimeout(() => {
    makeAnnouncement('Start!', 1000);
  }, 1000 * seconds + 1000);
  activeTimeouts.push(timeout);
}

export function clearAnnouncements() {
  activeTimeouts.forEach(timeout => clearTimeout(timeout));
  activeTimeouts = [];

  const elements = document.querySelectorAll('.announcement');
  elements.forEach(el => el.remove());
}