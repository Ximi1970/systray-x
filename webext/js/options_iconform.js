function fileSelected() {
  let input = document.getElementById("selectedFileIconType");

  if (input.files.length > 0) {
    console.debug("Selected file: " + input.files[0].name);
    console.debug("Selected file type: " + input.files[0].type);
  }

  function storeFile() {
    let buffer = new Uint8Array(fr.result);

    let binary = "";
    let len = buffer.byteLength;
    for (let i = 0; i < len; i++) {
      binary += String.fromCharCode(buffer[i]);
    }

    let base64 = window.btoa(binary);
    let iconDiv = document.getElementById("icon");
    iconDiv.setAttribute("data-icon", base64);
    iconDiv.setAttribute("data-icon-mime", input.files[0].type);

    let image = document.getElementById("customIconImage");
    image.setAttribute("src", `data:${input.files[0].type};base64,${base64}` );

    console.log(base64);
  }

  fr = new FileReader();
  fr.onload = storeFile;
  fr.readAsArrayBuffer(input.files[0]);
}

document
  .getElementById("selectedFileIconType")
  .addEventListener("change", fileSelected);
