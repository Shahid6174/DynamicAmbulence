document.addEventListener("DOMContentLoaded", () => {
    
  // Set current date and time
  const now = new Date()
  document.getElementById("date-time").textContent = now.toLocaleString()

  // Tab switching
  const tabBtns = document.querySelectorAll(".tab-btn")
  const tabContents = document.querySelectorAll(".tab-content")

  tabBtns.forEach((btn) => {
    btn.addEventListener("click", () => {
      const tabId = btn.getAttribute("data-tab")

      // Remove active class from all buttons and contents
      tabBtns.forEach((b) => b.classList.remove("active"))
      tabContents.forEach((c) => c.classList.remove("active"))

      // Add active class to current button and content
      btn.classList.add("active")
      document.getElementById(tabId).classList.add("active")
      
    })
  })
// let a=()=>destinationContainer.style.display ="block"
// a();
  // Toggle switches
  const emergencyToggle = document.getElementById("emergency")
  const emergencyStatus = document.getElementById("emergency-status")
  const destinationContainer = document.getElementById("destination-hospital-container")
//   destinationContainer.style.display ="block"

  emergencyToggle.addEventListener("change", function () {
    emergencyStatus.textContent = this.checked ? "Yes" : "No"
    destinationContainer.style.display = this.checked ? "none" : "block"
  })

  const vaccinesToggle = document.getElementById("vaccines")
  const vaccinesStatus = document.getElementById("vaccines-status")

  vaccinesToggle.addEventListener("change", function () {
    vaccinesStatus.textContent = this.checked ? "Yes" : "No"
  })

  const insuranceToggle = document.getElementById("insurance")
  const insuranceStatus = document.getElementById("insurance-status")

  insuranceToggle.addEventListener("change", function () {
    insuranceStatus.textContent = this.checked ? "Yes" : "No"
  })

  // Find Hospital button
  const findBtn = document.getElementById("find-btn")
  findBtn.addEventListener("click", findHospital)

  // Get Hospital Name button
  const getNameBtn = document.getElementById("get-name-btn")
  getNameBtn.addEventListener("click", getHospitalName)

  // Show Hospitals List button
  const showListBtn = document.getElementById("show-list-btn")
  showListBtn.addEventListener("click", showHospitalsList)

  // Patient form submission
  const patientForm = document.getElementById("patient-form")
  patientForm.addEventListener("submit", handlePatientFormSubmit)

  // Modal close buttons
  const modal = document.getElementById("modal")
  const closeBtn = document.querySelector(".close-btn")
  const modalCloseBtn = document.getElementById("modal-close-btn")

  closeBtn.addEventListener("click", () => {
    modal.style.display = "none"
  })

  modalCloseBtn.addEventListener("click", () => {
    modal.style.display = "none"
  })

  window.addEventListener("click", (e) => {
    if (e.target === modal) {
      modal.style.display = "none"
    }
  })

  // Load patient records if any
  loadPatientRecords()
})

// Define data arrays
const locationNames = ["Location 1", "Location 2", "Location 3", "Location 4", "Location 5"]

const hospitalNames = [
  "Hospital A",
  "Hospital B",
  "Hospital C",
  "Hospital D",
  "Hospital E",
  "Hospital F",
  "Hospital G",
  "Hospital H",
  "Hospital I",
  "Hospital J",
  "Hospital K",
  "Hospital L",
  "Hospital M",
  "Hospital N",
  "Hospital O",
]

const adjacencyMatrix = [
  [0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0],
]

const weightsMatrix = [
  [0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 3, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 4, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 5, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 6, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 7, 0, 8, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 8, 0, 9, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 9, 0, 10, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 11, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 12, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 13, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 14, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 15],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0],
]

const casualtiesMatrix = [
  [0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [10, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 20, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 30, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 40, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 50, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 60, 0, 70, 0, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 70, 0, 80, 0, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 80, 0, 90, 0, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 90, 0, 100, 0, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 110, 0, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 0, 120, 0, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 120, 0, 130, 0],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 130, 0, 140],
  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 140, 0],
]

const moneyFactor = 1.5

const patientRecords = JSON.parse(localStorage.getItem("patientRecords")) || []

// Find Hospital function
function findHospital() {
  const isEmergency = document.getElementById("emergency").checked
  const locationSelect = document.getElementById("location")
  const destinationSelect = document.getElementById("destination-hospital")
  const resultContainer = document.getElementById("result-container")
  const resultContent = document.getElementById("result-content")

  // Validate location selection
  if (locationSelect.value === "") {
    showModal("Error", "Please select your location.", "error")
    return
  }
  resultContainer.style.display = "inline"

  // Get source location index (0-based)
  const src = Number.parseInt(locationSelect.value) - 1

  if (isEmergency) {
    // Emergency case - find nearest hospital
    findNearestHospital(src, resultContent)
  } else {
    // Non-emergency case - find optimal route
    if (destinationSelect.value === "") {
      showModal("Error", "Please select your destination hospital.", "error")
      return
    }

    const dest = Number.parseInt(destinationSelect.value) - 1
    findOptimalRoute(src, dest, resultContent)
  }

  resultContainer.style.display = "block"
}

// Find nearest hospital function
function findNearestHospital(src, resultContent) {
  resultContent.innerHTML = `<p>Finding the nearest hospital possible from ${locationNames[src]}...</p>`

  let minWeight = Number.POSITIVE_INFINITY
  let nearestHospital = -1
  let casualtyHTML = ""

  // Check all connected hospitals
  for (let i = 0; i < 15; i++) {
    if (adjacencyMatrix[src][i] === 1) {
      const weight = weightsMatrix[src][i]
      const casualties = casualtiesMatrix[src][i]

      // Determine casualty level class
      let casualtyClass = ""
      let difficultyText = ""

      if (casualties >= 40) {
        casualtyClass = "casualty-high"
        difficultyText = "It is very difficult to get admitted. It is better if you try another hospital..."
      } else if (casualties >= 30) {
        casualtyClass = "casualty-medium"
        difficultyText = "You may get admitted but may have to wait..."
      } else {
        casualtyClass = "casualty-low"
        difficultyText = "You may get admitted easily..."
      }

      casualtyHTML += `
                <div class="casualty-info ${casualtyClass}">
                    <p><strong>Hospital:</strong> ${hospitalNames[i]}</p>
                    <p><strong>Casualty Level:</strong> ${casualties}</p>
                    <p><strong>Status:</strong> ${difficultyText}</p>
                </div>
            `

      if (weight < minWeight) {
        minWeight = weight
        nearestHospital = i
      }
    }
  }

  // Check if there's a self-loop with a smaller weight
  if (adjacencyMatrix[src][src] === 1 && weightsMatrix[src][src] < minWeight) {
    minWeight = weightsMatrix[src][src]
    nearestHospital = src
  }

  resultContent.innerHTML += casualtyHTML

  if (nearestHospital !== -1) {
    const optimalCost = minWeight * moneyFactor

    resultContent.innerHTML += `
            <div class="route-info">
                <h3>Nearest Hospital Found</h3>
                <p><strong>From:</strong> ${locationNames[src]}</p>
                <p><strong>To:</strong> ${hospitalNames[nearestHospital]}</p>
                <p><strong>Road Rating:</strong> ${minWeight}</p>
                <div class="cost-info">
                    <p>Optimal Cost: ₹${optimalCost.toFixed(2)}</p>
                </div>
            </div>
            <button id="proceed-btn" class="btn primary-btn">Proceed to Patient Details</button>
        `

    // Add event listener to the proceed button
    document.getElementById("proceed-btn").addEventListener("click", () => {
      showPatientForm(src, nearestHospital, minWeight)
    })
  } else {
    resultContent.innerHTML += `<p>No adjacent hospitals found for ${locationNames[src]}</p>`
  }
}

// Find optimal route function
function findOptimalRoute(src, dest, resultContent) {
  resultContent.innerHTML = `<p>Finding the optimal route from ${locationNames[src]} to ${hospitalNames[dest]}...</p>`

  // Implement Dijkstra's algorithm
  const distance = new Array(15).fill(Number.POSITIVE_INFINITY)
  const previous = new Array(15).fill(-1)
  const visited = new Array(15).fill(false)

  distance[src] = 0

  // Find the optimal route
  for (let count = 0; count < 15 - 1; count++) {
    let u = -1
    let minDistance = Number.POSITIVE_INFINITY

    // Select the node with the minimum distance
    for (let v = 0; v < 15; v++) {
      if (!visited[v] && distance[v] < minDistance) {
        u = v
        minDistance = distance[v]
      }
    }

    if (u === -1) break

    // Mark the selected node as visited
    visited[u] = true

    // Update distances of the adjacent nodes
    for (let v = 0; v < 15; v++) {
      if (adjacencyMatrix[u][v] === 1 && !visited[v] && distance[u] + weightsMatrix[u][v] < distance[v]) {
        distance[v] = distance[u] + weightsMatrix[u][v]
        previous[v] = u
      }
    }
  }

  // Build the path
  const path = []
  let current = dest

  while (current !== -1) {
    path.unshift(current)
    current = previous[current]
  }

  if (path.length > 0 && path[0] === src) {
    // Calculate average edge weight
    let totalWeight = 0
    for (let i = 0; i < path.length - 1; i++) {
      totalWeight += weightsMatrix[path[i]][path[i + 1]]
    }

    const averageWeight = totalWeight / (path.length - 1)
    const optimalCost = averageWeight * moneyFactor

    // Build the route path HTML
    let routePathHTML = '<div class="route-path">'

    for (let i = 0; i < path.length; i++) {
      routePathHTML += `<div class="route-node">${hospitalNames[path[i]]}</div>`

      if (i < path.length - 1) {
        routePathHTML += '<div class="route-arrow">→</div>'
      }
    }

    routePathHTML += "</div>"

    resultContent.innerHTML += `
            <div class="route-info">
                <h3>Optimal Route Found</h3>
                <p><strong>From:</strong> ${locationNames[src]}</p>
                <p><strong>To:</strong> ${hospitalNames[dest]}</p>
                <p><strong>Route:</strong></p>
                ${routePathHTML}
                <p><strong>Average Road Rating:</strong> ${averageWeight.toFixed(2)}</p>
                <div class="cost-info">
                    <p>Optimal Cost: ₹${optimalCost.toFixed(2)}</p>
                </div>
            </div>
            <button id="proceed-btn" class="btn primary-btn">Proceed to Patient Details</button>
        `

    // Add event listener to the proceed button
    document.getElementById("proceed-btn").addEventListener("click", () => {
      showPatientForm(src, dest, averageWeight)
    })
  } else {
    resultContent.innerHTML += `<p>No path found from ${locationNames[src]} to ${hospitalNames[dest]}</p>`
  }
}

// Get Hospital Name function
function getHospitalName() {
  const hospitalNumber = document.getElementById("hospital-number").value
  const resultContainer = document.getElementById("hospital-name-result")

  if (hospitalNumber === "" || hospitalNumber < 1 || hospitalNumber > 15) {
    showModal("Error", "Please enter a valid hospital number (1-15).", "error")
    return
  }

  const index = Number.parseInt(hospitalNumber) - 1
  resultContainer.innerHTML = `<p>Hospital ${hospitalNumber}: ${hospitalNames[index]}</p>`
  resultContainer.style.display = "block"
}

// Show Hospitals List function
function showHospitalsList() {
  const resultContainer = document.getElementById("hospitals-list-result")
  let html = "<h3>Hospitals and Their Connections</h3>"

  for (let i = 0; i < 15; i++) {
    html += `<div class="hospital-item"><p><strong>${i + 1}. ${hospitalNames[i]}</strong></p><p>Connected to: `

    const connections = []
    for (let j = 0; j < 15; j++) {
      if (adjacencyMatrix[i][j] === 1 && i !== j) {
        connections.push(
          `${hospitalNames[j]} (Road Rating: ${weightsMatrix[i][j]}, Casualties: ${casualtiesMatrix[i][j]})`,
        )
      }
    }

    if (connections.length > 0) {
      html += connections.join(", ")
    } else {
      html += "No connections"
    }

    html += "</p></div>"
  }

  resultContainer.innerHTML = html
  resultContainer.style.display = "block"
}

// Show Patient Form function
function showPatientForm(src, dest, averageWeight) {
  const patientFormContainer = document.getElementById("patient-form-container")

  // Store the data as attributes for later use
  patientFormContainer.setAttribute("data-src", src)
  patientFormContainer.setAttribute("data-dest", dest)
  patientFormContainer.setAttribute("data-weight", averageWeight)

  patientFormContainer.style.display = "block"

  // Scroll to the form
  patientFormContainer.scrollIntoView({ behavior: "smooth" })
}

// Handle Patient Form Submit function
function handlePatientFormSubmit(e) {
  e.preventDefault()

  const patientFormContainer = document.getElementById("patient-form-container")
  const src = Number.parseInt(patientFormContainer.getAttribute("data-src"))
  const dest = Number.parseInt(patientFormContainer.getAttribute("data-dest"))
  const averageWeight = Number.parseFloat(patientFormContainer.getAttribute("data-weight"))

  const hospitalId = document.getElementById("hospital-id").value

  // Check if hospital ID already exists
  if (isHospitalIdExists(hospitalId)) {
    showModal("Error", `Patient record with Hospital ID ${hospitalId} already exists.`, "error")
    return
  }

  // Get form values
  const patientName = document.getElementById("patient-name").value
  const patientAge = document.getElementById("patient-age").value
  const bloodGroup = document.getElementById("blood-group").value
  const vaccinesDone = document.getElementById("vaccines").checked ? "y" : "n"
  const treatmentArea = document.getElementById("treatment-area").value
  const insurance = document.getElementById("insurance").checked ? "yes" : "no"
  const phoneNumber = document.getElementById("phone-number").value

  // Calculate optimal cost
  const optimalCost = averageWeight * moneyFactor

  // Create patient record
  const patientRecord = {
    name: patientName,
    age: patientAge,
    bloodGroup: bloodGroup,
    hospitalId: hospitalId,
    vaccinesDone: vaccinesDone,
    treatmentArea: treatmentArea,
    insurance: insurance,
    phoneNumber: phoneNumber,
    hospitalAssigned: hospitalNames[dest],
    optimalCost: optimalCost,
    timestamp: new Date().toISOString(),
  }

  // Add to patient records
  patientRecords.push(patientRecord)

  // Save to localStorage
  localStorage.setItem("patientRecords", JSON.stringify(patientRecords))

  // Show success message
  showModal("Success", "Patient details successfully recorded.", "success Patient will be admitted to the hospital.", () => {
    // Reset form
    document.getElementById("patient-form").reset()

    // Hide form
    patientFormContainer.style.display = "none"

    // Show patient records
    showPatientRecords()
  })
}

// Check if Hospital ID exists function
function isHospitalIdExists(hospitalId) {
  return patientRecords.some((record) => record.hospitalId === hospitalId)
}

// Show Modal function
function showModal(title, message, type, callback) {
  const modal = document.getElementById("modal")
  const modalTitle = document.getElementById("modal-title")
  const modalBody = document.getElementById("modal-body")
  const modalActionBtn = document.getElementById("modal-action-btn")

  modalTitle.textContent = title
  modalBody.innerHTML = `<p>${message}</p>`

  if (type === "error") {
    modalActionBtn.style.display = "none"
  } else {
    modalActionBtn.style.display = "inline-block"
    modalActionBtn.textContent = "Continue"

    // Remove previous event listeners
    const newModalActionBtn = modalActionBtn.cloneNode(true)
    modalActionBtn.parentNode.replaceChild(newModalActionBtn, modalActionBtn)

    // Add new event listener
    newModalActionBtn.addEventListener("click", () => {
      modal.style.display = "none"
      if (callback) callback()
    })
  }

  modal.style.display = "block"
}

// Load Patient Records function
function loadPatientRecords() {
  if (patientRecords.length > 0) {
    showPatientRecords()
  }
}

// Show Patient Records function
function showPatientRecords() {
  const patientRecordsContainer = document.getElementById("patient-records-container")
  const patientRecordsDiv = document.getElementById("patient-records")

  let html = ""

  patientRecords.forEach((record) => {
    html += `
            <div class="patient-record">
                <h3>Patient ID: ${record.hospitalId}</h3>
                <div class="patient-detail">
                    <span class="detail-label">Name:</span>
                    <span class="detail-value">${record.name}</span>
                </div>
                <div class="patient-detail">
                    <span class="detail-label">Age:</span>
                    <span class="detail-value">${record.age}</span>
                </div>
                <div class="patient-detail">
                    <span class="detail-label">Blood Group:</span>
                    <span class="detail-value">${record.bloodGroup}</span>
                </div>
                <div class="patient-detail">
                    <span class="detail-label">Vaccines Done:</span>
                    <span class="detail-value">${record.vaccinesDone === "y" ? "Yes" : "No"}</span>
                </div>
                <div class="patient-detail">
                    <span class="detail-label">Treatment Area:</span>
                    <span class="detail-value">${record.treatmentArea}</span>
                </div>
                <div class="patient-detail">
                    <span class="detail-label">Insurance:</span>
                    <span class="detail-value">${record.insurance}</span>
                </div>
                <div class="patient-detail">
                    <span class="detail-label">Phone Number:</span>
                    <span class="detail-value">${record.phoneNumber}</span>
                </div>
                <div class="patient-detail">
                    <span class="detail-label">Hospital Assigned:</span>
                    <span class="detail-value">${record.hospitalAssigned}</span>
                </div>
                <div class="patient-detail">
                    <span class="detail-label">Optimal Cost:</span>
                    <span class="detail-value">₹${record.optimalCost.toFixed(2)}</span>
                </div>
            </div>
        `
  })

  patientRecordsDiv.innerHTML = html
  patientRecordsContainer.style.display = "block"
}
