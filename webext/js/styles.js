var stylesheet = document.currentScript.getAttribute('stylesheet'); 
document.getElementsByTagName("head")[0].insertAdjacentHTML(
    "beforeend",
    `<link rel=\"stylesheet\" href=${stylesheet} />`);
