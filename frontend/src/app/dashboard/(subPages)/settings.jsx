import Link from 'next/link';

export default function SettingsPage() {
  return (
    <div id="settings-view" className="view">
      <div className="container-fluid">
        <div className="d-flex justify-content-between align-items-center mb-4">
          <h2>Settings</h2>
          <nav aria-label="breadcrumb">
            <ol className="breadcrumb">
              <li className="breadcrumb-item">
                <Link href="/"><span>Dashboard</span></Link>
              </li>
              <li className="breadcrumb-item active">Settings</li>
            </ol>
          </nav>
        </div>

        <div className="row mb-4">
          <div className="col-md-12">
            <div className="card shadow-sm">
              <div className="card-header bg-secondary text-white d-flex align-items-center">
                <i className="bi bi-gear-fill me-2 display-6"></i>
                <h5 className="mb-0">Application Settings</h5>
              </div>
              <div className="card-body">
                {/* Settings form or options go here */}
                <p className="text-muted mb-0">Settings options will appear here.</p>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
