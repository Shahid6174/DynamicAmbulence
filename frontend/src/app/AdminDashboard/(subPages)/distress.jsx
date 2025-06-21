'use client';

import { useState } from 'react';

export default function DistressCenter({ calls = [] }) {
  const [data, setData] = useState([
    {
      name: 'Raj Patel',
      timestamp: Date.now(),
      location: 'Yelahanka',
      hospital: 'Sparsh Hospital',
      status: 'assigned',
      confidence: 92,
    },
    {
      name: 'Sita Verma',
      timestamp: Date.now() - 200000,
      location: 'Bannerghatta',
      hospital: '',
      status: 'pending',
      confidence: 66,
    },
    {
      name: 'John Doe',
      timestamp: Date.now() - 450000,
      location: 'Unknown',
      hospital: 'Apollo Hospital',
      status: 'completed',
      confidence: 88,
    },
  ]);

  const statusColor = {
    pending: 'secondary',
    assigned: 'primary',
    completed: 'success',
    failed: 'danger',
  };

  return (
    <div className="container-fluid py-4">
      <div className="d-flex justify-content-between align-items-center mb-4">
        <h4 className="fw-bold text-danger mb-0">
          <i className="bi bi-exclamation-triangle-fill me-2"></i>
          Distress Center
        </h4>
        <span className="text-muted small">
          Monitor live emergency calls and AI hospital assignments
        </span>
      </div>

      <div className="card shadow-sm">
        <div className="card-body table-responsive">
          {data.length === 0 ? (
            <p className="text-muted text-center mb-0">No distress calls yet.</p>
          ) : (
            <table className="table table-hover align-middle small">
              <thead className="table-light">
                <tr>
                  <th>#</th>
                  <th>Patient</th>
                  <th>Time</th>
                  <th>Location</th>
                  <th>Assigned Hospital</th>
                  <th>Status</th>
                  <th>AI Confidence</th>
                  <th className="text-end">Action</th>
                </tr>
              </thead>
              <tbody>
                {data.map((call, i) => (
                  <tr key={i}>
                    <td>{i + 1}</td>
                    <td>
                      <i className="bi bi-person-circle me-2 text-primary"></i>
                      {call.name}
                    </td>
                    <td>{new Date(call.timestamp).toLocaleString()}</td>
                    <td>{call.location || 'Unknown'}</td>
                    <td>
                      {call.hospital || <span className="text-muted">Not assigned</span>}
                    </td>
                    <td>
                      <span className={`badge bg-${statusColor[call.status] || 'secondary'}`}>
                        {call.status}
                      </span>
                    </td>
                    <td>
                      <span
                        className={`badge rounded-pill bg-opacity-75 ${
                          call.confidence > 85
                            ? 'bg-success'
                            : call.confidence > 60
                            ? 'bg-warning text-dark'
                            : 'bg-danger'
                        }`}
                      >
                        {call.confidence}%
                      </span>
                    </td>
                    <td className="text-end">
                      <button className="btn btn-sm btn-outline-info">
                        <i className="bi bi-eye me-1"></i>View
                      </button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
      </div>
    </div>
  );
}
