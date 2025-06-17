import Link from 'next/link';

export default function AppointmentsPage() {
  return (
    <div id="appointments-view" className="view">
      <div className="container-fluid">
        <div className="d-flex justify-content-between align-items-center mb-4">
          <h2>Appointments</h2>
          <nav aria-label="breadcrumb">
            <ol className="breadcrumb">
              <li className="breadcrumb-item">
                <Link href="/"><span>Dashboard</span></Link>
              </li>
              <li className="breadcrumb-item active">Appointments</li>
            </ol>
          </nav>
        </div>

        <div className="row mb-4">
          <div className="col-md-12">
            <div className="card shadow-sm">
              <div className="card-header bg-primary text-white d-flex align-items-center">
                <i className="bi bi-calendar-check-fill me-2 display-6"></i>
                <h5 className="mb-0">Upcoming Appointments</h5>
              </div>
              <div className="card-body">
                <div id="appointments-list">
                  {/* Appointments list or table goes here */}
                  <p className="text-muted mb-0">No appointments scheduled.</p>
                </div>
              </div>
            </div>
          </div>
        </div>

        <div className="d-flex gap-2">
          <Link href="/add_appointment">
            <button className="btn btn--primary">
              <i className="bi bi-calendar-plus me-2"></i>Schedule Appointment
            </button>
          </Link>
        </div>
      </div>
    </div>
  );
}
