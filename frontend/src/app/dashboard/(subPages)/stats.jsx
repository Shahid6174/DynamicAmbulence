import Link from 'next/link';
export default function MainApp (props){
  return (
      <div id="dashboard-view" className="view">
        <div className="container-fluid">
          <div className="d-flex justify-content-between align-items-center mb-4">
            <h2>Dashboard</h2>
            <nav aria-label="breadcrumb">
              <ol className="breadcrumb">
                <li className="breadcrumb-item active">Dashboard</li>
              </ol>
            </nav>
          </div>

          <div className="row mb-4">
            <div className="col-md-3 mb-3">
              <div className="card stats-card bg-primary text-white">
                <div className="card-body">
                  <div className="d-flex justify-content-between align-items-center">
                    <div>
                      <h3 className="card-title h2 mb-1">124</h3>
                      <p className="card-text">Total Patients</p>
                    </div>
                    <i className="bi bi-people-fill display-6"></i>
                  </div>
                </div>
              </div>
            </div>
            <div className="col-md-3 mb-3">
              <div className="card stats-card bg-success text-white">
                <div className="card-body">
                  <div className="d-flex justify-content-between align-items-center">
                    <div>
                      <h3 className="card-title h2 mb-1">36</h3>
                      <p className="card-text">Appointments</p>
                    </div>
                    <i className="bi bi-calendar-check-fill display-6"></i>
                  </div>
                </div>
              </div>
            </div>
            <div className="col-md-3 mb-3">
              <div className="card stats-card bg-warning text-white">
                <div className="card-body">
                  <div className="d-flex justify-content-between align-items-center">
                    <div>
                      <h3 className="card-title h2 mb-1">15</h3>
                      <p className="card-text">Doctors</p>
                    </div>
                    <i className="bi bi-person-badge-fill display-6"></i>
                  </div>
                </div>
              </div>
            </div>
            <div className="col-md-3 mb-3">
              <div className="card stats-card bg-info text-white">
                <div className="card-body">
                  <div className="d-flex justify-content-between align-items-center">
                    <div>
                      <h3 className="card-title h2 mb-1">$12.4k</h3>
                      <p className="card-text">Revenue</p>
                    </div>
                    <i className="bi bi-cash-stack display-6"></i>
                  </div>
                </div>
              </div>
            </div>
          </div>

          <div className="row">
            <div className="col-md-8">
              <div className="card">
                <div className="card-header">
                  <h5 className="card-title mb-0">Recent Patients</h5>
                </div>
                <div className="card-body">
                  <div id="recent-patients-list"></div>
                </div>
              </div>
            </div>
            <div className="col-md-4">
              <div className="card">
                <div className="card-header">
                  <h5 className="card-title mb-0">Quick Actions</h5>
                </div>
                <div className="card-body">
                  <div className="d-grid gap-2">

                  <Link href="/add_patient">
                    <button
                      className="btn btn--secondary">
                      <i className="bi bi-person-plus me-2"></i>Add New Patient
                      </button>
                   </Link>
                   
                    <button
                      className="btn btn--secondary"
                      onClick={() => navigateTo('appointments')}
                    >
                      <i className="bi bi-calendar-plus me-2"></i>Schedule Appointment
                    </button>
                    <button
                      className="btn btn--outline"
                      onClick={() => navigateTo('reports')}
                    >
                      <i className="bi bi-file-medical me-2"></i>View Reports
                    </button>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>

    );
  };
