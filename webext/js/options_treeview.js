var toggler = document.getElementsByClassName("caret");
var i;

for (i = 0; i < toggler.length; i++) {
  toggler[i].addEventListener("click", function() {
    this.parentElement.querySelector(".nested").classList.toggle("active");
    this.classList.toggle("caret-down");
  });
}

var togglerBox = document.getElementsByClassName("box");

for (i = 0; i < togglerBox.length; i++) {
  togglerBox[i].addEventListener("click", function() {
    this.classList.toggle("check-box");
  });
}
