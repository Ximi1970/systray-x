function pollStartupDelay(e) {
//  console.debug("Startup: " + e.target.value);
}

function pollInterval(e) {
//  console.debug("Interval: " + e.target.value);
}

document
  .getElementById("pollStartupDelay")
  .addEventListener("change", pollStartupDelay);

document
  .getElementById("pollInterval")
  .addEventListener("change", pollInterval);
