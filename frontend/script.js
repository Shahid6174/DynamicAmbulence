document.addEventListener("DOMContentLoaded", function () {
  const loginForm = document.getElementById("loginForm");
  if (loginForm) {
    loginForm.addEventListener("submit", function (e) {
      e.preventDefault();
      window.location.href = "dashboard.html";
    });
  }
});

const patientSearchInput = document.getElementById("patientSearch");
if (patientSearchInput) {
  patientSearchInput.addEventListener("input", function () {
    const filter = this.value.toLowerCase();
    const patientRows = document.querySelectorAll(".patient-row");
    patientRows.forEach(row => {
      const name = row.querySelector(".patient-name").textContent.toLowerCase();
      row.style.display = name.includes(filter) ? "" : "none";
    });
  });
}

const addPatientForm = document.getElementById("addPatientForm");
if (addPatientForm) {
  addPatientForm.addEventListener("submit", function (e) {
    e.preventDefault();
    const name = document.getElementById("name").value.trim();
    const age = document.getElementById("age").value.trim();
    const condition = document.getElementById("condition").value.trim();

    if (!name || !age || !condition) {
      alert("Please fill in all fields");
      return;
    }

    alert("Patient added successfully!");
    this.reset();
  });
}

const emergencyForm = document.getElementById("emergencyForm");
if (emergencyForm) {
  emergencyForm.addEventListener("submit", function (e) {
    e.preventDefault();
    const location = document.getElementById("location").value.trim();
    const severity = document.getElementById("severity").value.trim();

    if (!location || !severity) {
      alert("Please fill in location and severity");
      return;
    }

    alert("Emergency submitted successfully! Help is on the way.");
    this.reset();
  });
}
