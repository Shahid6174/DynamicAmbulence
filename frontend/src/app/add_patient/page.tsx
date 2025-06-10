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
                        <label htmlFor="patient-name">Full Name *</label>
                      </div>
                    </div>
                    <div className="col-md-3">
                      <div className="form-floating">
                        <input type="number" className="form-control" id="patient-age" min="1" max="120" placeholder="Age" required />
                        <label htmlFor="patient-age">Age *</label>
                      </div>
                    </div>
                    <div className="col-md-3">
                      <div className="form-floating">
                        <select className="form-select" id="patient-gender" required>
                          <option value="">Select Gender</option>
                          <option value="Male">Male</option>
                          <option value="Female">Female</option>
                          <option value="Other">Other</option>
                        </select>
                        <label htmlFor="patient-gender">Gender *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <input type="tel" className="form-control" id="patient-contact" placeholder="Contact Number" required />
                        <label htmlFor="patient-contact">Contact Number *</label>
                      </div>
                    </div>
                    <div className="col-md-6">
                      <div className="form-floating">
                        <input type="email" className="form-control" id="patient-email" placeholder="Email Address" />
                        <label htmlFor="patient-email">Email Address</label>
                      </div>
                    </div>
                    <div className="col-12">
                      <div className="form-floating">
                        <textarea className="form-control" id="patient-address" placeholder="Address" style={{height: '80px'}}></textarea>
                        <label htmlFor="patient-address">Address</label>
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
                      onClick={() => navigateTo('patients')}
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
