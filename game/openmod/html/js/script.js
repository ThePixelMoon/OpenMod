const buttons = document.querySelectorAll('.top-btn');

buttons.forEach(button => {
  button.addEventListener('mouseenter', () => {
    const hoverSound = new Audio('assets/ui/hover.mp3');
      
    hoverSound.preload = 'auto';
      
    hoverSound.play().catch(error => { }); // don't catch an error
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