function fileSelectedDefault() {
  const input = document.getElementById("selectedFileDefaultIconType");

  //  if (input.files.length > 0) {
  //    console.debug("Selected file: " + input.files[0].name);
  //    console.debug("Selected file type: " + input.files[0].type);
  //  }

  function storeFile() {
    const buffer = new Uint8Array(fr.result);

    let binary = "";
    const len = buffer.byteLength;
    for (let i = 0; i < len; i++) {
      binary += String.fromCharCode(buffer[i]);
    }

    const base64 = window.btoa(binary);
    const iconDiv = document.getElementById("defaultIcon");
    iconDiv.setAttribute("data-default-icon", base64);
    iconDiv.setAttribute("data-default-icon-mime", input.files[0].type);

    const image = document.getElementById("defaultCustomIconImage");
    image.setAttribute("src", `data:${input.files[0].type};base64,${base64}`);
  }

  fr = new FileReader();
  fr.onload = storeFile;
  fr.readAsArrayBuffer(input.files[0]);
}

function fileSelected() {
  const input = document.getElementById("selectedFileIconType");

  //  if (input.files.length > 0) {
  //    console.debug("Selected file: " + input.files[0].name);
  //    console.debug("Selected file type: " + input.files[0].type);
  //  }

  function storeFile() {
    const buffer = new Uint8Array(fr.result);

    let binary = "";
    const len = buffer.byteLength;
    for (let i = 0; i < len; i++) {
      binary += String.fromCharCode(buffer[i]);
    }

    const base64 = window.btoa(binary);
    const iconDiv = document.getElementById("icon");
    iconDiv.setAttribute("data-icon", base64);
    iconDiv.setAttribute("data-icon-mime", input.files[0].type);

    const image = document.getElementById("customIconImage");
    image.setAttribute("src", `data:${input.files[0].type};base64,${base64}`);
  }

  fr = new FileReader();
  fr.onload = storeFile;
  fr.readAsArrayBuffer(input.files[0]);
}

document
  .getElementById("selectedFileIconType")
  .addEventListener("change", fileSelected);

document
  .getElementById("selectedFileDefaultIconType")
  .addEventListener("change", fileSelectedDefault);
