'use client';

import { useRouter } from 'next/navigation';

export default function MainApp() {
  const router = useRouter();

  const navigateTo = (route) => {
    router.push(`/${route}`);
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    alert('Patient saved!');
  };

  return (
    <div id="add-patient-view" className="view bg-light min-vh-100 py-5">
      <div className="container">
        <div className="d-flex justify-content-between align-items-center mb-4">
          <h2 className="fw-bold text-primary mb-0">Add New Patient</h2>
          <nav aria-label="breadcrumb">
            <ol className="breadcrumb mb-0 bg-white px-3 py-2 rounded shadow-sm">
              <li className="breadcrumb-item">
                <a href="#dashboard" className="text-decoration-none text-primary">Dashboard</a>
              </li>
              <li className="breadcrumb-item">
                <a href="#patients" className="text-decoration-none text-primary">Patients</a>
              </li>
              <li className="breadcrumb-item active" aria-current="page">Add Patient</li>
            </ol>
          </nav>
        </div>

        <div className="row justify-content-center">
          <div className="col-lg-8">
            <div className="card shadow rounded-4 border-0">
              <div className="card-body p-4">
                <form id="add-patient-form" onSubmit={handleSubmit} autoComplete="off">
                  <div className="row g-3">
                    <div className="col-md-6">
                      <div className="form-floating">
                        <input type="text" className="form-control" id="patient-name" placeholder="Full Name" required />
                        <label htmlFor="patient-name">Name *</label>
                      </div>
                    </div>
                    <div className="col-md-3">
                      <div className="form-floating">
                        <input type="number" className="form-control" id="patient-age" min="0" max="120" placeholder="Age" required />
                        <label htmlFor="patient-age">Age *</label>
                      </div>
                    </div>
                    <div className="col-md-3">
                      <div className="form-floating">
                        <select className="form-select" id="patient-blood" required>
                          <option value="">Select</option>
                          <option value="A+">A+</option>
                          <option value="A-">A-</option>
                          <option value="B+">B+</option>
                          <option value="B-">B-</option>
                          <option value="AB+">AB+</option>
                          <option value="AB-">AB-</option>
                          <option value="O+">O+</option>
                          <option value="O-">O-</option>
                        </select>
                        <label htmlFor="patient-blood">Blood Group *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <input type="number" className="form-control" id="patient-id" placeholder="Patient ID" required />
                        <label htmlFor="patient-id">Patient ID *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <select className="form-select" id="patient-vaccines" required>
                          <option value="">Select</option>
                          <option value="y">Yes</option>
                          <option value="n">No</option>
                        </select>
                        <label htmlFor="patient-vaccines">Vaccines Done *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <input type="text" className="form-control" id="patient-area" placeholder="Area of Treatment" required />
                        <label htmlFor="patient-area">Area of Treatment *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <select className="form-select" id="patient-insurance" required>
                          <option value="">Select</option>
                          <option value="yes">Yes</option>
                          <option value="no">No</option>
                        </select>
                        <label htmlFor="patient-insurance">Insurance *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <input type="tel" className="form-control" id="patient-contact" placeholder="Phone Number" required />
                        <label htmlFor="patient-contact">Phone Number *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <input type="text" className="form-control" id="patient-hospital" placeholder="Hospital Assigned" required />
                        <label htmlFor="patient-hospital">Hospital Assigned *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <input type="number" step="0.01" className="form-control" id="patient-cost" placeholder="Optimal Cost" required />
                        <label htmlFor="patient-cost">Optimal Cost (INR) *</label>
                      </div>
                    </div>
                  </div>
                  <div className="d-flex gap-3 mt-4">
                    <button type="submit" className="btn btn-primary px-4 shadow-sm">
                      <i className="bi bi-save me-2"></i>Save Patient
                    </button>
                    <button
                      type="button"
                      className="btn btn-outline-secondary px-4"
                      onClick={() => navigateTo('AdminDashboard')}
                    >
                      <i className="bi bi-x me-2"></i>Cancel
                    </button>
                  </div>
                </form>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
