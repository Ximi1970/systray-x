function openTab(evt) {
  const tabName = evt.currentTarget.id;

  const tabcontent = document.getElementsByClassName("tabcontent");
  for (let i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }
  const tablinks = document.getElementsByClassName("tablinks");
  for (let i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }

  const el = document.getElementById(tabName + "Content");
  el.style.display = "block";

  evt.currentTarget.className += " active";
}

document.addEventListener("DOMContentLoaded", function() {
  const tablinks = document.getElementsByClassName("tablinks");
  for (let i = 0; i < tablinks.length; i++) {
    document.getElementById(tablinks[i].id).addEventListener("click", openTab);
  }
});
