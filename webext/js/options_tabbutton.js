function openTab(evt) {
    var i, tabcontent, tablinks, tabName;
    tabName = evt.currentTarget.id;

    tabcontent = document.getElementsByClassName('tabcontent');
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = 'none';
    }
    tablinks = document.getElementsByClassName('tablinks');
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].className = tablinks[i].className.replace(' active', '');
    }

    let el = document.getElementById(tabName+'Content');
    el.style.display = 'block';

    evt.currentTarget.className += ' active';
}

document.addEventListener('DOMContentLoaded', function () {
  let tablinks = document.getElementsByClassName('tablinks');
  for (i = 0; i < tablinks.length; i++) {
    document.getElementById(tablinks[i].id)
      .addEventListener('click', openTab);
  }
});

