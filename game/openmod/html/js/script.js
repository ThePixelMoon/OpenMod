const buttons = document.querySelectorAll('.top-btn');

buttons.forEach(button => {
  button.addEventListener('mouseenter', () => {
    const hoverSound = new Audio('assets/ui/hover.wav');
    hoverSound.preload = 'auto';
    hoverSound.play().catch(error => { }); // don't catch an error
  });

  button.addEventListener('click', () => {
    const clickSound = new Audio('assets/ui/click.wav');
    clickSound.preload = 'auto';
    clickSound.play().catch(error => { }); // don't catch an error
  });
});

let gameState = 'menu';

function handleButtonAction() {
  if (gameState === 'inGame') {
    alert('cmd:gamemenucommand Disconnect');
  } else if (gameState === 'menu') {
    alert('cmd:gamemenucommand Quit');
  }
}

function togglevisible(show) {
  gameState = show ? "inGame" : "menu";
}

function openPopup(url) {
  alert(`cmd:open "${url}"`);
}

const playPanel = document.querySelector('.play-panel');
const newsPanel = document.querySelector('.news-panel');
const bottomButtons = document.querySelector('.bottom-buttons');

playPanel.style.transition = 'opacity 0.2s';
newsPanel.style.transition = 'opacity 0.2s';
bottomButtons.style.transition = 'opacity 0.2s';

function setVisible(huh) {
  playPanel.style.opacity = huh ? '1' : '0';
  newsPanel.style.opacity = huh ? '0' : '1';
  bottomButtons.style.opacity = huh ? '1' : '0';

  if (huh == true) {
    playPanel.style.display = 'block';
    bottomButtons.style.display = 'none';
    newsPanel.style.display = 'none';
  } else {
    newsPanel.style.display = 'block';
    bottomButtons.style.display = 'flex';
    playPanel.style.display = 'none';
  }
}
