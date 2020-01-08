var tree = document.querySelectorAll('ul.tree a:not(:last-child)');
for(var i = 0; i < tree.length; i++){
  tree[i].addEventListener('click', function(e) {
    var parent = e.target.parentElement;
    var classList = parent.classList;
    if(classList.contains("open")) {
      classList.remove('open');
      var opensubs = parent.querySelectorAll(':scope .open');
      for(var i = 0; i < opensubs.length; i++){
        opensubs[i].classList.remove('open');
      }
    } else {
      classList.add('open');
    }
    e.preventDefault();
  });
}
